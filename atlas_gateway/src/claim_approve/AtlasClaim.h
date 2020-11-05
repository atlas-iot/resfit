#ifndef __ATLAS_CLAIM_H__
#define __ATLAS_CLAIM_H__

#include "../alarm/AtlasAlarm.h"
#include "../http/AtlasHttpServer.h"

namespace atlas {

class AtlasClaim {

public:
    /**
    * @brief Get instance for claiming module
    * @return Claiming module instance
    */
    static AtlasClaim& getInstance();

    /**
     * @brief Start claim protocol
     * @return true if claim protocol is started, false otherwise
     */
    bool start();

private:

    /**
     * @brief Ctor for claiming protocol
     * @return none
     */
    AtlasClaim();

    /**
     * @brief Short code alarm callback
     * @return none
     */
    void alarmCallback();

    /**
     * @brief Handle claim request
     * @param[in] method HTTP method
     * @param[in] path HTTP path
     * @param[in] payload Claim request payload (JSON format which contains the following fields: short code,
     * claim secret key, owner identifier)
     * @return HTTP response
     */
    AtlasHttpResponse handleClaimReq(AtlasHttpMethod method, const std::string &path,
                                     const std::string &payload);


    /**
     * @brief Generate short code
     * @return short code
     */
    std::string generateShortCode();

    /* Short code alarm */
    AtlasAlarm shortCodeAlarm_;

    /* HTTP claim request callback */
    AtlasHttpCallback claimCallback_;

    /* Short code used for authenticating claim requests */
    std::string shortCode_;
};

} // namespace atlas

#endif /* __ATLAS_CLAIM_H__ */
