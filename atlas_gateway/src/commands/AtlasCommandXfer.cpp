#include <arpa/inet.h>
#include "AtlasCommandXfer.h"
#include "AtlasCommand.h"

namespace atlas {

AtlasCommandXfer::AtlasCommandXfer()
{
    init();
}

void AtlasCommandXfer::init()
{
    cmdXferState_ = AtlasCommandXferState::ATLAS_CMD_XFER_HEADER_STATE;
    cmdLen_ = 0;
    cmdRemainingLen_ = ATLAS_CMD_HEADER_LEN;
}

void AtlasCommandXfer::handleArrivedData(size_t bytesTransferred)
{
    uint16_t len;

    cmdRemainingLen_ -= bytesTransferred;
    cmdLen_ += bytesTransferred;
    
    /* Command read FSM */
    switch(cmdXferState_) {
        case AtlasCommandXferState::ATLAS_CMD_XFER_HEADER_STATE:
            if (!cmdRemainingLen_) {
                /* Read command length (skip type) */
                memcpy(&len, data_ + sizeof(uint16_t), sizeof(uint16_t));
                cmdRemainingLen_ = ntohs(len);
                /* Read payload now */
                cmdXferState_ = AtlasCommandXferState::ATLAS_CMD_XFER_PAYLOAD_STATE;
            }

            break;
 
        case AtlasCommandXferState::ATLAS_CMD_XFER_PAYLOAD_STATE:
            break;
    }
}

} // namespace atlas
