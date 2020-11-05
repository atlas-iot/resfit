#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <utility>
#include <arpa/inet.h>
#include "AtlasFilter.h"
#include "../logger/AtlasLogger.h"
#include "../../commands/AtlasCommand.h"
#include "../../commands/AtlasCommandBatch.h"

namespace atlas {

const int ATLAS_SOCKET_RECONNECT_RETRY_SEC = 10;
const int ATLAS_FIXED_WINDOW_RATE_LIMIT_INTERVAL_MIN = 1;

AtlasFilter::AtlasFilter() : work_(ioService_), socket_(nullptr),
                             timer_(ioService_, boost::posix_time::minutes(ATLAS_FIXED_WINDOW_RATE_LIMIT_INTERVAL_MIN))
{
    timer_.async_wait(boost::bind(&AtlasFilter::timerHandler, this, _1));    
}

AtlasFilter& AtlasFilter::getInstance()
{
    static AtlasFilter instance;

    return instance;
}

void AtlasFilter::timerHandler(const boost::system::error_code& ec)
{
    ATLAS_LOGGER_DEBUG("Fixed window rate limit timer handler executed");
 
    if (!ec) {
        /* Start timer again */
        timer_.expires_at(timer_.expires_at() + boost::posix_time::minutes(ATLAS_FIXED_WINDOW_RATE_LIMIT_INTERVAL_MIN));
        timer_.async_wait(boost::bind(&AtlasFilter::timerHandler, this, _1));

        /* Start a new rate limit window */
        mutex_.lock();

        std::unordered_map<std::string, AtlasPacketPolicer>::iterator it = rules_.begin();
        while(it != rules_.end()) {
	    (*it).second.rateLimitWindowStart();
            it++;
        }

        mutex_.unlock();
    } else
        ATLAS_LOGGER_ERROR("Timer handler called with error");
}

void AtlasFilter::writeFirewallStats(const std::string &clientId, uint32_t ruleDroppedPkts, uint32_t rulePassedPkts,
                                     uint32_t txDroppedPkts, uint32_t txPassedPkts)
{
    AtlasCommandBatch cmdBatchInner;
    AtlasCommandBatch cmdBatchOuter;

    /* Inner commands */
    ruleDroppedPkts = htonl(ruleDroppedPkts);
    rulePassedPkts = htonl(rulePassedPkts);
    txDroppedPkts = htonl(txDroppedPkts);
    txPassedPkts = htonl(txPassedPkts);

    AtlasCommand cmdClientId(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *) clientId.c_str());
    AtlasCommand cmdRuleDroppedPkts(ATLAS_CMD_PUB_SUB_PKT_DROP, sizeof(uint32_t), (uint8_t *) &ruleDroppedPkts);
    AtlasCommand cmdRulePassedPkts(ATLAS_CMD_PUB_SUB_PKT_PASS, sizeof(uint32_t), (uint8_t *) &rulePassedPkts);
    AtlasCommand cmdTxDroppedPkts(ATLAS_CMD_PUB_SUB_TX_PKT_DROP, sizeof(uint32_t), (uint8_t *) &txDroppedPkts);
    AtlasCommand cmdTxPassedPkts(ATLAS_CMD_PUB_SUB_TX_PKT_PASS, sizeof(uint32_t), (uint8_t *) &txPassedPkts);
    
    cmdBatchInner.addCommand(cmdClientId);
    cmdBatchInner.addCommand(cmdRuleDroppedPkts);
    cmdBatchInner.addCommand(cmdRulePassedPkts);
    cmdBatchInner.addCommand(cmdTxDroppedPkts);
    cmdBatchInner.addCommand(cmdTxPassedPkts);

    std::pair<const uint8_t*, size_t> innerCmd = cmdBatchInner.getSerializedAddedCommands();

    /* Outer commands */
    AtlasCommand cmdPutStatRule(ATLAS_CMD_PUB_SUB_PUT_STAT_FIREWALL_RULE, innerCmd.second, innerCmd.first);

    cmdBatchOuter.addCommand(cmdPutStatRule);

    std::pair<const uint8_t*, size_t> outerCmd = cmdBatchOuter.getSerializedAddedCommands();

    /* Write command to gateway */
    boost::asio::async_write(*socket_,
                             boost::asio::buffer(outerCmd.first, outerCmd.second),
                             boost::bind(&AtlasFilter::handleWrite, this, _1));

    ATLAS_LOGGER_DEBUG("Firewall rule statistics sent by agent");
}

void AtlasFilter::getFirewallRuleStats(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";
    uint32_t ruleDroppedPkts = 0;
    uint32_t rulePassedPkts = 0;
    uint32_t txDroppedPkts = 0;
    uint32_t txPassedPkts = 0;

    ATLAS_LOGGER_DEBUG("Get firewall rule statistics from agent");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID)
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
    }
    
    if (clientId == "") {
        ATLAS_LOGGER_ERROR("Empty client id in firewall get statistics command");
        return;
    }

    /* Get statistics for firewall rule */
    mutex_.lock();
    ruleDroppedPkts = rules_[clientId].getStatDroppedPkt();
    rules_[clientId].setStatDroppedPkt(0);
    rulePassedPkts = rules_[clientId].getStatPassedPkt();
    rules_[clientId].setStatPassedPkt(0);
    txDroppedPkts = txPktStats_[clientId].getDroppedPkts();
    txPktStats_[clientId].setDroppedPkts(0);
    txPassedPkts = txPktStats_[clientId].getPassedPkts();
    txPktStats_[clientId].setPassedPkts(0);
    mutex_.unlock();
        
    writeFirewallStats(clientId, ruleDroppedPkts, rulePassedPkts, txDroppedPkts, txPassedPkts);
} 

void AtlasFilter::removeFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";

    ATLAS_LOGGER_DEBUG("Remove firewall rule from agent");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds)
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID)
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
 
    if (clientId == "") {    
        ATLAS_LOGGER_ERROR("Empty client id in firewall remove command. Dropping command...");
        return;
    }

    /* Remove firewall rule */
    mutex_.lock();
    rules_.erase(clientId);
    txPktStats_.erase(clientId);
    mutex_.unlock();

    ATLAS_LOGGER_INFO("Firewall rule for client id %s was removed", clientId.c_str());
} 

void AtlasFilter::installFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";
    uint16_t maxQos;
    uint16_t ppm;
    uint16_t maxPayloadLen;
    bool qosFound = false;
    bool ppmFound = false;
    bool payloadLenFound = false;

    ATLAS_LOGGER_DEBUG("Install firewall rule from gateway");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID) {
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_MAX_QOS && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&maxQos, cmd.getVal(), sizeof(uint16_t));
            maxQos = ntohs(maxQos);
            qosFound = true;
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_PPM && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&ppm, cmd.getVal(), sizeof(uint16_t));
            ppm = ntohs(ppm);
            ppmFound = true;      
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&maxPayloadLen, cmd.getVal(), sizeof(uint16_t));
            maxPayloadLen = ntohs(maxPayloadLen);
            payloadLenFound = true;
        }
    }

    if (clientId == "") {    
        ATLAS_LOGGER_ERROR("Empty client id in firewall install command. Dropping command...");
        return;
    }

    if (!qosFound) {    
        ATLAS_LOGGER_ERROR("Empty QoS in firewall install command. Dropping command...");
        return;
    }

    if (!ppmFound) {    
        ATLAS_LOGGER_ERROR("Empty PPM in firewall install command. Dropping command...");
        return;
    }

    if (!payloadLenFound) {    
        ATLAS_LOGGER_ERROR("Empty payload length in firewall install command. Dropping command...");
        return;
    }

    /* Install firewall rule */
    mutex_.lock();
    rules_[clientId] = AtlasPacketPolicer(maxQos, ppm, maxPayloadLen);
    txPktStats_[clientId] = AtlasPacketStats();
    mutex_.unlock();
    
    ATLAS_LOGGER_INFO("Installed firewall rule for client id %s, Max QoS: %d, PPM: %d, Max payload length: %d",
                      clientId.c_str(), maxQos, ppm, maxPayloadLen);
}

void AtlasFilter::processCommand(const uint8_t *cmdBuffer, size_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    
    ATLAS_LOGGER_DEBUG("Process command from gateway");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuffer, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE)
            installFirewallRule(cmd.getVal(), cmd.getLen());
        else if (cmd.getType() == ATLAS_CMD_PUB_SUB_REMOVE_FIREWALL_RULE)
            removeFirewallRule(cmd.getVal(), cmd.getLen());
        else if (cmd.getType() == ATLAS_CMD_PUB_SUB_GET_STAT_FIREWALL_RULE)
            getFirewallRuleStats(cmd.getVal(), cmd.getLen());
    }
}

void AtlasFilter::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from gateway");
       
        /* Handle arrived data */ 
        cmdXfer_.handleArrivedData(bytesTransferred);
        /* Process command if all the data is available */
        if (cmdXfer_.isCmdAvailable()) {
            processCommand(cmdXfer_.getDataBuffer(), cmdXfer_.getCmdLen());
            cmdXfer_.init();
        }

        socket_->async_read_some(boost::asio::buffer(cmdXfer_.getReadBuffer(),
                                                     cmdXfer_.getRemainingLen()),
                                 boost::bind(&AtlasFilter::handleRead, this, _1, _2));
    } else {
        ATLAS_LOGGER_ERROR("Error when reading data from gateway. Trying to reconnect to the gateway...");
        gatewayConnect();
    }
}

void AtlasFilter::handleWrite(const boost::system::error_code& error)
{
    if (error)
        ATLAS_LOGGER_DEBUG("Error when writing to gateway");
}

void AtlasFilter::gatewayConnect()
{
    mutex_.lock();
    ATLAS_LOGGER_INFO("Remove all unmanaged rules in firewall");
    rules_.clear();
    mutex_.unlock();

    while(true) {
        try {
            boost::asio::local::stream_protocol::endpoint ep(ATLAS_PUB_SUB_AGENT_SOCK);
            delete socket_;
            socket_ = new boost::asio::local::stream_protocol::socket(ioService_);
            /* Connect to gateway */
            socket_->connect(ep);

            cmdXfer_.init();
            socket_->async_read_some(boost::asio::buffer(cmdXfer_.getReadBuffer(),
                                                         cmdXfer_.getRemainingLen()),
                                     boost::bind(&AtlasFilter::handleRead, this, _1, _2));

            /* Get all firewall rules */
            AtlasCommandBatch cmdBatch;
            AtlasCommand getFwRulesCmd(ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES, 0, nullptr);
            cmdBatch.addCommand(getFwRulesCmd);
            std::pair<const uint8_t*, size_t> serializedCmd = cmdBatch.getSerializedAddedCommands();

            boost::asio::async_write(*socket_,
                                     boost::asio::buffer(serializedCmd.first, serializedCmd.second),
                                     boost::bind(&AtlasFilter::handleWrite, this, _1));

            ATLAS_LOGGER_DEBUG("Publish-subscribe agent is connected to the gateway");
            break;
        } catch (std::exception &e) {
            ATLAS_LOGGER_DEBUG("An exception occured when trying to connect to the gateway: %s", e.what());
            std::this_thread::sleep_for (std::chrono::seconds(ATLAS_SOCKET_RECONNECT_RETRY_SEC));
        }
    }
}

void AtlasFilter::gatewayLoop()
{
    ATLAS_LOGGER_DEBUG("Starting gateway connection main loop...");
    
    gatewayConnect();

    ioService_.run();
}

void AtlasFilter::init()
{
    std::thread t(&AtlasFilter::gatewayLoop, this);
    
    t.detach();
}

bool AtlasFilter::filter(const AtlasPacket &pkt)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string dstClientId = pkt.getDstClientId();
    bool processPkt = false;
    

    /* If no rule is found for the destination client id, then packet can be processed */
    if (rules_.find(dstClientId) == rules_.end())
        processPkt = true;
    else
        processPkt = rules_[dstClientId].filter(pkt);

    /* Increment TX statistics */
    if (pkt.getSrcClientId() &&
        txPktStats_.find(pkt.getSrcClientId()) != txPktStats_.end()) {
        if (processPkt)
            txPktStats_[pkt.getSrcClientId()].incPassedPkts();
        else
            txPktStats_[pkt.getSrcClientId()].incDroppedPkts();
    }


    return processPkt;
}

} // namespace atlas
