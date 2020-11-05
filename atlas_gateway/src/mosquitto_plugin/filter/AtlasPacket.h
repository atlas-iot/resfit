#ifndef __ATLAS_PACKET_H__
#define __ATLAS_PACKET_H__

#include <stdint.h>

namespace atlas {

class AtlasPacket
{

public:
    /**
     * @brief Set packet source username
     * @param[in] username Username
     * @return none
     */
    inline void setSrcUsername(const char *username) { srcUsername_ = username; }
    
    /**
     * @brief Set packet source client id
     * @param[in] clientId Client id
     * @return none
     */
    inline void setSrcClientId(const char *clientId) { srcClientId_ = clientId; }
    
    /**
     * @brief Set packet destination username
     * @param[in] username Username
     * @return none
     */
    inline void setDstUsername(const char *username) { dstUsername_ = username; }
   
    /**
     * @brief Set packet destination client id
     * @param[in] clientId Client id
     * @return none
     */ 
    inline void setDstClientId(const char *clientId) {dstClientId_ = clientId; }
    
    /**
     * @brief Set packet topic
     * @param[in] topic Topic
     * @return none
     */
    inline void setTopic(const char *topic) { topic_ = topic; }
    
    /**
     * @brief Set packet QoS
     * @param[in] qos QoS
     * @return none
     */
    inline void setQos(uint8_t qos) { qos_ = qos; }
    
    /**
     * @brief Set packet payload
     * @param[in] payload Payload
     * @return none
     */
    inline void setPayload(const uint8_t *payload) { payload_ = payload; }
    
    /**
     * @brief Set packet payload length
     * @param[in] payloadLen Payload length
     * @return none
     */
    inline void setPayloadLen(uint16_t payloadLen) { payloadLen_ = payloadLen; }

    /**
     * @brief Get packet source username
     * @return Source username
     */
    inline const char *getSrcUsername() const { return srcUsername_; }

    /**
     * @brief Get packet source client id
     * @return Source client id
     */    
    inline const char *getSrcClientId() const { return srcClientId_; }
    
    /**
     * @brief Get packet destination username
     * @return Destination username
     */
    inline const char *getDstUsername() const { return dstUsername_; }
    
    /**
    * @brief Get packet destination client id
    * @return Destination client id
    */
    inline const char *getDstClientId() const { return dstClientId_; }
    
    /**
     * @brief Get packet topic
     * @return Topic
     */
    inline const char *getTopic() const { return topic_; }
    
    /**
     * @brief Get packet QoS
     * @return QoS
     */
    inline uint8_t getQos() const { return qos_; }
    
    /**
     * @brief Get packet payload
     * @return Payload
     *
     */
    inline const uint8_t *getPayload() const { return payload_; }
    
    /**
     * @brief Get packet payload length
     * @return Payload length
     */
    inline uint16_t getPayloadLen() const { return payloadLen_; }

private:
    /* Packet source username */
    const char *srcUsername_;

    /* Packet source client id */
    const char *srcClientId_;

    /* Packet destination username */
    const char *dstUsername_;
    
    /* Packet destination client id */
    const char *dstClientId_;
    
    /* Packet topic */
    const char *topic_;
    
    /* Packet QoS */
    uint8_t qos_;
    
    /* Packet payload */
    const uint8_t *payload_;
    
    /* Packet payload length */
    uint16_t payloadLen_;
};

} // namespace atlas

#endif /* __ATLAS_PACKET_H__ */
