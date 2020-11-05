#include <arpa/inet.h>
#include <string.h>
#include "AtlasCommandBatch.h"

namespace atlas {

std::pair<const uint8_t*, size_t> AtlasCommandBatch::getSerializedAddedCommands()
{
    std::pair<const uint8_t*, size_t> ret(nullptr, 0);
    uint8_t *ptr;
    size_t totalLen = 0;

    if (addedCmd_.empty())
        return ret;

    if (addedCmdBuf_)
        delete[] addedCmdBuf_;

    for (const AtlasCommand &cmd : addedCmd_) {
        totalLen += 2 * sizeof(uint16_t) + cmd.getLen();
    }

    addedCmdBuf_ = new uint8_t[totalLen];
    memset(addedCmdBuf_, 0, totalLen);

    ptr = addedCmdBuf_;
    for (const AtlasCommand &cmd : addedCmd_) {
        /* Set type */
        uint16_t type = cmd.getType();
        type = htons(type);
        memcpy(ptr, &type, sizeof(type));
        ptr += sizeof(type);
    
        /* Set length */
        uint16_t len = cmd.getLen();
        len = htons(len);
        memcpy(ptr, &len, sizeof(len));
        ptr += sizeof(len);

        /* Set value */
        if (cmd.getVal())
            memcpy(ptr, cmd.getVal(), cmd.getLen());
        
        ptr += cmd.getLen();
    }
    
    ret.first = addedCmdBuf_;
    ret.second = totalLen;

    return ret;    
}

void AtlasCommandBatch::addCommand(const AtlasCommand &cmd)
{
    addedCmd_.push_back(cmd);
}

void AtlasCommandBatch::parseCommands(size_t cmdLen)
{
    uint8_t *ptr = parsedCmdBuf_;

    while (cmdLen > 0) {

        /* Get command type */
        if (cmdLen < sizeof(uint16_t))
            return;

        uint16_t type = *((uint16_t *) ptr);
        type = ntohs(type);
        ptr += sizeof(uint16_t);
        cmdLen -= sizeof(uint16_t);

        /* Get command length */
        if (cmdLen < sizeof(uint16_t))
            return;

        uint16_t len = *((uint16_t *) ptr);
        len = ntohs(len);
        ptr += sizeof(uint16_t);
        cmdLen -= sizeof(uint16_t);

        /* Get command value */
        if (cmdLen < len)
            return;
        
        /* Save command */
        parsedCmd_.push_back(AtlasCommand(type, len, ptr));
 
        ptr += len;
        cmdLen -= len;
    }
}

void AtlasCommandBatch::setRawCommands(const uint8_t *cmd, size_t cmdLen)
{
    if (!cmd || !cmdLen)
        return;

    /* Clear existing commands, if any */
    if (parsedCmdBuf_)
        delete[] parsedCmdBuf_;

    parsedCmd_.clear();

    parsedCmdBuf_ = new uint8_t[cmdLen];
    memcpy(parsedCmdBuf_, cmd, cmdLen);

    parseCommands(cmdLen);
}

AtlasCommandBatch::~AtlasCommandBatch()
{
    delete[] parsedCmdBuf_;
    delete[] addedCmdBuf_;
}

} //namespace atlas
