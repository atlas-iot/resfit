#ifndef __ATLAS_COMMAND_XFER_H__
#define __ATLAS_COMMAND_XFER_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "AtlasCommandXferState.h"

namespace atlas {

namespace {

const int ATLAS_PUB_SUB_AGENT_BUF_LEN = 8192;

} // anonymous namespace

class AtlasCommandXfer
{

public:
    /**
    * @brief Ctor for command transfer
    * @return none
    */
    AtlasCommandXfer();

    /**
    * @brief Init command transfer logic
    * @return none
    */
    void init();

    /**
    * @brief Handle data arrival (execute internal xfer FSM)
    * @param[in] bytesTransferred Number of bytes transffered
    * @return none
    */
    void handleArrivedData(size_t bytesTransferred);

    /**
    * @brief Get command data buffer
    * @return Pointer to command data buffer
    */
    inline uint8_t *getDataBuffer() { return data_; }
    
    /**
    * @brief Get command read buffer
    * @return Pointer to command read buffer
    */
    inline uint8_t *getReadBuffer() { return data_ + cmdLen_; }

    /**
    * @brief Get command length
    * @return Command length
    */
    inline size_t getCmdLen() const { return cmdLen_; }

    /**
    * @brief Get the number of command remaining bytes
    * @return Number of remaining bytes
    */
    inline size_t getRemainingLen() const { return cmdRemainingLen_; }

    /**
    * @brief Verify if full command is availabe
    * @return True if command is available, false otherwise
    */
    inline bool isCmdAvailable() const { return cmdXferState_ == AtlasCommandXferState::ATLAS_CMD_XFER_PAYLOAD_STATE && !cmdRemainingLen_; }

private:
    /* Read data buffer */
    uint8_t data_[ATLAS_PUB_SUB_AGENT_BUF_LEN];

    /* Commnand transfer state */
    AtlasCommandXferState cmdXferState_;

    /* Current command length */
    size_t cmdLen_;

    /* Number of bytes remaining for the command transfer */
    size_t cmdRemainingLen_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_XFER_H__ */
