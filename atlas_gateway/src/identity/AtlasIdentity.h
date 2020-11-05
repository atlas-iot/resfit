#ifndef __ATLAS_IDENTITY_H__
#define __ATLAS_IDENTITY_H__

#include <uuid/uuid.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "../logger/AtlasLogger.h"

namespace atlas {

class AtlasIdentity
{

public:
    /**
     * @brief Get singleton instance for identity
     * @return Indentity instance
     */
    static AtlasIdentity &getInstance();

    /**
    * @brief Get client device identity
    * @return Client device identity
    */
    inline std::string getIdentity() const { return identity_; }

    /**
    * @brief Get client device pre-shared key 
    * @return Client device pre-shared key
    */
    inline std::string getPsk() const { return psk_; }

    AtlasIdentity(const AtlasIdentity &) = delete;
    AtlasIdentity & operator=(const AtlasIdentity &) = delete;

    /**
    * @brief Initialize identity/psk
    * @return true if initialized with success, otherwise false
    */
    bool initIdentity();

private:
    /**
    * @brief Default ctor for client device
    * @return none
    */
    AtlasIdentity() {};

    /**
    * @brief Generate gateway identity
    * @return true if generated with success, otherwise false
    */
    bool generateIdentity();

    /**
    * @brief Generate gateway psk
    * @return true if generated with success, otherwise false
    */
    bool generatePsk();

    /**
    * @brief Transform to base64
    * @return none
    */
    void to_base64(const uint8_t *, size_t, char *, size_t);

    /* Gateway identity */
    std::string identity_;

    /* Gateway pre-shared key */
    std::string psk_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_H__ */
