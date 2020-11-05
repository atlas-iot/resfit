#ifndef __ATLAS_PACKET_POLICER_H__
#define __ATLAS_PACKET_POLICER_H__

#include <stdint.h>
#include "AtlasPacketPolicer.h"
#include "AtlasPacket.h"

namespace atlas {

class AtlasPacketPolicer
{

public:

    /**
    * @brief Default ctor for packet policer
    * @return none
    */
    AtlasPacketPolicer() {}

    /**
    * @brief Ctor for packet policer
    * @param[in] maxQoS Max QoS value
    * @param[in] ppm Allowed packets per minute
    * @param[in] maxPayloadLen Max payload length
    * @return none
    */
    AtlasPacketPolicer(uint8_t maxQos, uint16_t ppm, uint16_t maxPayloadLen);
    
    /**
    * @brief Filter publis-subscribe packet
    * @param[in] pkt Publish-subscribe packet
    * @return True if packet should be processed, False if packet should be dropped
    */
    bool filter(const AtlasPacket &pkt);

    /**
    * @brief Get number of dropped packets
    * @return Number of dropped packets
    */
    uint32_t getStatDroppedPkt() const { return statPktDrop_; }

    /**
    * @brief Get number of passed packets
    * @return Number of passed packets
    */
    uint32_t getStatPassedPkt() const { return statPktPass_; }

    /**
    * @brief Set number of dropped packets
    * @return none
    */
    inline void setStatDroppedPkt(uint32_t statPktDrop) { statPktDrop_ = statPktDrop; }

    /**
    * @brief Set number of passed packets
    * @return none
    */
    inline void setStatPassedPkt(uint32_t statPktPass) { statPktPass_ = statPktPass; }

    /**
    * @brief Start a new window for the rate limit (packets per minute)
    * @return none
    */
    void rateLimitWindowStart();

private:
    /* Rate limit current packet counter per window */
    uint32_t windowPpm_;
    
    /* Qualifiers */
    
    /* Maximum QoS value */
    uint8_t maxQos_;
    
    /* Allowed number of packets per minute */
    uint16_t ppm_;

    /* Maximum payload length*/
    uint16_t maxPayloadLen_;

    /* Statistics */

    /* Number of dropped packets */
    uint32_t statPktDrop_;

    /* Number of passed packets */
    uint32_t statPktPass_;
};

} // namespace atlas

#endif /* __ATLAS_PACKET_POLICER_H__ */
