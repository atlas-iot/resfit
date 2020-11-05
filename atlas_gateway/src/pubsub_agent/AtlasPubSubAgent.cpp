#include <iostream>
#include <boost/optional.hpp>
#include "AtlasPubSubAgent.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../commands/AtlasCommand.h"
#include "../device/AtlasDeviceManager.h"
#include "../policy/AtlasFirewallPolicy.h"
#include "../sql/AtlasSQLite.h"

namespace atlas {


AtlasPubSubAgent& AtlasPubSubAgent::getInstance()
{
    static AtlasPubSubAgent instance;

    return instance;
}

AtlasPubSubAgent::AtlasPubSubAgent() : acceptor_(AtlasScheduler::getInstance().getService()),
                                       acceptingSocket_(nullptr), connectedSocket_(nullptr)
{
    boost::asio::local::stream_protocol::endpoint ep(ATLAS_PUB_SUB_AGENT_SOCK);

    /* Remove previous binding */
    ::unlink(ATLAS_PUB_SUB_AGENT_SOCK.c_str());

    /* Bind acceptor to local unix socket */
    acceptor_.open(ep.protocol());
    acceptor_.bind(ep);
    acceptor_.listen();
}

AtlasPubSubAgent::~AtlasPubSubAgent()
{
    acceptor_.cancel();
    acceptor_.close();
    delete acceptingSocket_;
    delete connectedSocket_;
}

void AtlasPubSubAgent::getFirewallRuleStats(const std::string &clientId)
{
    AtlasCommandBatch cmdBatchInner;
    AtlasCommandBatch cmdBatchOuter;
    
    if (clientId == "") {
        ATLAS_LOGGER_ERROR("Empty client id is not allowed when getting statistics fora firewall rule");
        return;
    }

    ATLAS_LOGGER_INFO1("Get statistics for firewall rule associated with client id ", clientId);

    /* Inner command */
    AtlasCommand cmdClientId(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *) clientId.c_str());
   
    cmdBatchInner.addCommand(cmdClientId);
    std::pair<const uint8_t*, size_t> innerCmd = cmdBatchInner.getSerializedAddedCommands();

    /* Outer command */
    AtlasCommand cmdRemoveFwRule(ATLAS_CMD_PUB_SUB_GET_STAT_FIREWALL_RULE, innerCmd.second, innerCmd.first);
    
    cmdBatchOuter.addCommand(cmdRemoveFwRule);

    std::pair<const uint8_t*, size_t> outerCmd = cmdBatchOuter.getSerializedAddedCommands();

    /* Write command to publish-subscribe agent */
    write(outerCmd.first, outerCmd.second);
}

void AtlasPubSubAgent::removeFirewallRule(const std::string &clientId)
{
    AtlasCommandBatch cmdBatchInner;
    AtlasCommandBatch cmdBatchOuter;
    
    if (clientId == "") {
        ATLAS_LOGGER_ERROR("Empty client id is not allowed when removing a firewall rule");
        return;
    }

    ATLAS_LOGGER_INFO1("Remove firewall rule for client id ", clientId);

    /* Inner command */
    AtlasCommand cmdClientId(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *) clientId.c_str());
   
    cmdBatchInner.addCommand(cmdClientId);
    std::pair<const uint8_t*, size_t> innerCmd = cmdBatchInner.getSerializedAddedCommands();

    /* Outer command */
    AtlasCommand cmdRemoveFwRule(ATLAS_CMD_PUB_SUB_REMOVE_FIREWALL_RULE, innerCmd.second, innerCmd.first);
    
    cmdBatchOuter.addCommand(cmdRemoveFwRule);

    std::pair<const uint8_t*, size_t> outerCmd = cmdBatchOuter.getSerializedAddedCommands();

    /* Write command to publish-subscribe agent */
    write(outerCmd.first, outerCmd.second);
}

void AtlasPubSubAgent::getAllFirewallRules()
{
    /* This is called by plug-in when it is up*/
    ATLAS_LOGGER_DEBUG("Get all firewall rules for publish-subscribe agent");

    AtlasDeviceManager::getInstance().installAllPolicies();
}

void AtlasPubSubAgent::processFirewallRuleStat(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";
    boost::optional<uint32_t> ruleDroppedPkts;
    boost::optional<uint32_t> rulePassedPkts;
    boost::optional<uint32_t> txDroppedPkts;
    boost::optional<uint32_t> txPassedPkts;
    uint32_t tmp;

    if (!cmdBuf || !cmdLen)
        return;

    ATLAS_LOGGER_DEBUG("Process firewall rule statistics from publish-subscribe agent");

    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID) {
            clientId.assign((char *) cmd.getVal(), cmd.getLen());
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_PKT_DROP && cmd.getLen() == sizeof(uint32_t)) {
            memcpy(&tmp, cmd.getVal(), cmd.getLen());
            tmp = ntohl(tmp);
            ruleDroppedPkts = tmp;
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_PKT_PASS && cmd.getLen() == sizeof(uint32_t)) {
            memcpy(&tmp, cmd.getVal(), cmd.getLen());
            tmp = ntohl(tmp);
            rulePassedPkts = tmp;
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_TX_PKT_DROP && cmd.getLen() == sizeof(uint32_t)) {
            memcpy(&tmp, cmd.getVal(), cmd.getLen());
            tmp = ntohl(tmp);
            txDroppedPkts = tmp;
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_TX_PKT_PASS && cmd.getLen() == sizeof(uint32_t)) {
            memcpy(&tmp, cmd.getVal(), cmd.getLen());
            tmp = ntohl(tmp);
            txPassedPkts = tmp;
        }
    }

    if (clientId == "") {
        ATLAS_LOGGER_ERROR("Empty client id found when processing firewall rule statistics command");
        return;
    }

    if (!ruleDroppedPkts) {
        ATLAS_LOGGER_ERROR("Empty dropped packets found when processing firewall rule statistics command");
        return;
    }

    if (!rulePassedPkts) {
        ATLAS_LOGGER_ERROR("Empty accepted packets found when processing firewall rule statistics command");
        return;
    }

    if (!txDroppedPkts) {
        ATLAS_LOGGER_ERROR("Empty TX dropped packets found when processing firewall rule statistics command");
        return;
    }

    if (!txPassedPkts) {
        ATLAS_LOGGER_ERROR("Empty TX accepted packets found when processing firewall rule statistics command");
        return;
    }

    /* If at least one value is not zero, update the device firewall statistics
    and sync the info with the cloud back-end */
    if (*ruleDroppedPkts || *rulePassedPkts ||
        *txDroppedPkts || *txPassedPkts) {
        AtlasFirewallStats firewallStats;

        firewallStats.setClientId(clientId);        
        firewallStats.addRuleDroppedPkts(*ruleDroppedPkts);
        firewallStats.addRulePassedPkts(*rulePassedPkts);
        firewallStats.addTxDroppedPkts(*txDroppedPkts);
        firewallStats.addTxPassedPkts(*txPassedPkts);
 
        /* Update firewall statistics with delta measurements */ 
        AtlasDeviceManager::getInstance().updateFirewallStats(firewallStats); 
    }
}

void AtlasPubSubAgent::processCommand(const uint8_t *cmdBuffer, size_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;

    ATLAS_LOGGER_DEBUG("Process command from publish-subscribe agent");

    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuffer, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES)
            getAllFirewallRules();
        else if (cmd.getType() == ATLAS_CMD_PUB_SUB_PUT_STAT_FIREWALL_RULE)
            processFirewallRuleStat(cmd.getVal(), cmd.getLen());
    }
}

void AtlasPubSubAgent::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from publish-subscribe agent");
 
        /* Handle arrived data */ 
        cmdXfer_.handleArrivedData(bytesTransferred);
        /* Process command if all the data is available */
        if (cmdXfer_.isCmdAvailable()) {
            processCommand(cmdXfer_.getDataBuffer(), cmdXfer_.getCmdLen());
            cmdXfer_.init();
        }
        
        connectedSocket_->async_read_some(boost::asio::buffer(cmdXfer_.getReadBuffer(),
                                                              cmdXfer_.getRemainingLen()),
                                          boost::bind(&AtlasPubSubAgent::handleRead, this, _1, _2));
    } else
        ATLAS_LOGGER_ERROR("Error when reading data from publish-subscribe agent. Closing socket...");
}

void AtlasPubSubAgent::handleWrite(const boost::system::error_code& error)
{
    if (error)
        ATLAS_LOGGER_DEBUG("Error when writing to publish-subscribe agent. Closing socket...");
}

void AtlasPubSubAgent::start()
{
    acceptingSocket_ = new boost::asio::local::stream_protocol::socket(AtlasScheduler::getInstance().getService()); 

    acceptor_.async_accept(*acceptingSocket_, boost::bind(&AtlasPubSubAgent::handleAccept, this, _1));

    ATLAS_LOGGER_INFO("Start listenting for local publish-subscribe agents...");
}

void AtlasPubSubAgent::handleAccept(const boost::system::error_code& error)
{
    ATLAS_LOGGER_DEBUG("Connection handler for publish-subscribe agent called");

    if (!error) {
        ATLAS_LOGGER_DEBUG("Accepting connection from publish-subscribe agent");
        /* Allow only one connection from agent, so close the existing one */
        delete connectedSocket_;
        connectedSocket_ = acceptingSocket_;
       
        cmdXfer_.init();
        connectedSocket_->async_read_some(boost::asio::buffer(cmdXfer_.getReadBuffer(),
                                                              cmdXfer_.getRemainingLen()),
                                          boost::bind(&AtlasPubSubAgent::handleRead, this, _1, _2));

    } else {
        ATLAS_LOGGER_DEBUG("Error in accepting connection from publish-subscribe agent ");
        delete acceptingSocket_;
        acceptingSocket_ = nullptr;
    }

    ATLAS_LOGGER_DEBUG("Start listening for a new connection...");

    start();
}

void AtlasPubSubAgent::write(const uint8_t *buf, size_t bufLen)
{
    if (!buf || !bufLen)
        return;

    ATLAS_LOGGER_DEBUG("Writing data to publish-subscribe agent.");

    if (connectedSocket_)
        boost::asio::async_write(*connectedSocket_, boost::asio::buffer(buf, bufLen),
                                 boost::bind(&AtlasPubSubAgent::handleWrite, this, _1));
}

void AtlasPubSubAgent::installFirewallRule(const std::string &identity, const AtlasFirewallPolicy *policy)
{
    if(!policy) {
        ATLAS_LOGGER_ERROR("Received an empty policy!");
        return;
    }

    ATLAS_LOGGER_DEBUG("Get firewall rule and forward through publish-subscribe agent");

    AtlasCommandBatch cmdBatchInner, cmdBatchOuter;

    uint16_t qos, ppm, payloadLen;
    std::string clientId;

    clientId = policy->getClientId();
    qos = htons(policy->getQOS());
    ppm = htons(policy->getPPM());
    payloadLen = htons(policy->getPayloadLen());

    AtlasCommand cmd1(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *)clientId.c_str());
    AtlasCommand cmd2(ATLAS_CMD_PUB_SUB_MAX_QOS, sizeof(qos), (uint8_t *)&qos);
    AtlasCommand cmd3(ATLAS_CMD_PUB_SUB_PPM, sizeof(ppm), (uint8_t *)&ppm);
    AtlasCommand cmd4(ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN, sizeof(payloadLen), (uint8_t *)&payloadLen);

    cmdBatchInner.addCommand(cmd1);
    cmdBatchInner.addCommand(cmd2);
    cmdBatchInner.addCommand(cmd3);
    cmdBatchInner.addCommand(cmd4);
    std::pair<const uint8_t*, size_t> inner = cmdBatchInner.getSerializedAddedCommands();

    AtlasCommand cmd5(ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE, inner.second, inner.first);
    cmdBatchOuter.addCommand(cmd5);
    std::pair<const uint8_t*, size_t> outer = cmdBatchOuter.getSerializedAddedCommands();

    write(outer.first, outer.second);
}

} // namespace atlas
