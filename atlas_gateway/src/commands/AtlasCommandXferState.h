#ifndef __ATLAS_COMMAND_XFER_STATE_H__
#define __ATLAS_COMMAND_XFER_STATE_H__

namespace atlas {

enum AtlasCommandXferState
{
    /* Read command header state */
    ATLAS_CMD_XFER_HEADER_STATE = 0,
    
    /* Read command payload state */
    ATLAS_CMD_XFER_PAYLOAD_STATE
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_XFER_STATE_H__ */
