#ifndef __ATLAS_PACKET_STATS_H__
#define __ATLAS_PACKET_STATS_H__

#include <stdint.h>

namespace atlas {

class AtlasPacketStats
{
public:

    /**
    * @bried Default ctor for packet statistics
    * @return none
    */
    AtlasPacketStats() : passedPkts_(0), droppedPkts_(0) {}

    /**
    * @brief Increment number of passed packets
    * @return none
    */
    inline void incPassedPkts() { ++passedPkts_; }
    
    /**
    * @brief Increment number of dropped packets
    * @return none
    */
    inline void incDroppedPkts() { ++droppedPkts_; }
    
    /**
    * @brief Set number of passed packets
    * @param[in] passedPkts Number of passed packets
    * @return none
    */
    inline void setPassedPkts(uint32_t passedPkts) { passedPkts_ = passedPkts; }
    
    /**
    * @brief Set number of dropped packets
    * @param[in] droppedPkts Number of dropped packets
    * @return none
    */
    inline void setDroppedPkts(uint32_t droppedPkts) { droppedPkts_ = droppedPkts; }

    /**
    * @brief Get number of passed packets
    * @return Number of passed packets
    */
    inline uint32_t getPassedPkts() const { return passedPkts_; }

    /**
    * @brief Get number of dropped packets
    * @return Number of dropped packets
    */
    inline uint32_t getDroppedPkts() const { return droppedPkts_; }

private:

    /* Number of passed (routed) packets */
    uint32_t passedPkts_;

    /* Number of dropped packets */
    uint32_t droppedPkts_;
};

} // namespace atlas

#endif /* __ATLAS_PACKET_STATS_H__ */
