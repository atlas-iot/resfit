#include "AtlasPacketPolicer.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

AtlasPacketPolicer::AtlasPacketPolicer(uint8_t maxQos, uint16_t ppm, uint16_t maxPayloadLen) : windowPpm_(0), statPktDrop_(0), statPktPass_(0)
{
    maxQos_ = maxQos;
    ppm_ = ppm;
    maxPayloadLen_ = maxPayloadLen;
}

bool AtlasPacketPolicer::filter(const AtlasPacket &pkt)
{
    /* Increment rate limit window counter */
    windowPpm_++;

    if (pkt.getQos() > maxQos_)
        goto DROP;

    if (pkt.getPayloadLen() > maxPayloadLen_)
        goto DROP;

    if (windowPpm_ > ppm_)
        goto DROP;

    /* Accept packet */
    ATLAS_LOGGER_DEBUG("Packet for destination client id %s is allowed", pkt.getDstClientId());
    statPktPass_++;
    return true;

DROP:
    ATLAS_LOGGER_DEBUG("Packet for destination client id %s is dropped", pkt.getDstClientId());
    /* Drop packet */
    statPktDrop_++;
    return false;
}

void AtlasPacketPolicer::rateLimitWindowStart()
{
    ATLAS_LOGGER_DEBUG("Start new window for rate limiting");

    windowPpm_ = 0;
}

} // namespace atlas
