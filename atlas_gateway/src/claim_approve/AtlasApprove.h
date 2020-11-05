#ifndef __ATLAS_APPROVE_H__
#define __ATLAS_APPROVE_H__

#include <string>
#include <json/json.h>
#include "../alarm/AtlasAlarm.h"

namespace atlas {

class AtlasApprove
{
public:
    /**
    * @brief Get instance for device approve module
    * @return Device approve module instance
    */
    static AtlasApprove& getInstance();

    /**
     * @brief Start approve protocol
     * @return none
     */
    void start();

    /**
     * @brief Stop approve protocol
     * @return none
     */
    void stop();

    /**
    * @brief Check approved cmd payload received from cloud back-end
    * @param[in] payload Command payload
    * @return true if the command was handled without error, false otherwise
    */
    bool handleClientCommand(const Json::Value &payload);
    
    /**
    * @brief Handle command DONE ACK message
    * @param[in] payload Command payload
    * @return none
    */
    void handleCommandDoneAck(const Json::Value &payload);

    /**
    * @brief Response with an ACK status to cloud back-end for a specific sequence number
    * @return true if the command was notified without error, false otherwise
    */
    bool responseCommandACK();

    /**
    * @brief Response with a DONE status to cloud back-end for a specific sequence number
    * @param[in] deviceIdentity Client device identity associated with this command
    * @return true if the command was notified without error, false otherwise
    */
    bool responseCommandDONE(const std::string &deviceIdentity);

    /**
    * @brief Set sequenceNumber on this gateway
    * @param[in] cmdSeqNo Sequence number
    * @return none
    */
    inline void setSequenceNumber(uint32_t cmdSeqNo) { sequenceNumber_ = cmdSeqNo; }

    AtlasApprove(const AtlasApprove &) = delete;
    AtlasApprove& operator=(const AtlasApprove &) = delete;

private:
    /**
    * @brief Default ctor for device approve module
    * @return none
    */
    AtlasApprove();

    /**
     * @brief Handle old command
     * @param[in] payload Cloud command payload
     * @return True if command was handled successfully, false otherwise
     */
    bool handleOldCommand(const Json::Value &payload);

    /**
     * @brief Push top-command alarm callback
     * @return none
     */
    void alarmCallback();

    /**
     * @brief Cloud sync ACK status alarm callback
     * @return none
     */
    void statusACKCallback();

    /**
     * @brief Validate command signature
     * @param[in] commandPayload Command payload
     * @return True if signature is valid, false otherwise
     */
    bool validateCommandSignature(const Json::Value &commandPayload);

    /* Push top-command alarm */
    AtlasAlarm pushCommandAlarm_;

    /* Cloud sync ACK status alarm */
    AtlasAlarm statusACKAlarm_;

    /* Sequence number of the last command */
    uint32_t sequenceNumber_;

    /* status for ACK alarm scheduling */
    bool msgACKScheduled_;

    /* counter for resending ACK messages */
    uint8_t counterACK_;
};

} // namespace atlas

#endif /* __ATLAS_APPROVE_H__ */
