#include <iostream>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include "AtlasIdentity.h"
#include "../logger/AtlasLogger.h"

namespace fs = std::experimental::filesystem; 

namespace atlas {

namespace {
    const std::string ATLAS_IDENTITY_FILE   = "atlas_gateway.identity";
    const std::string ATLAS_PSK_FILE        = "atlas_gateway.psk";
    const int ATLAS_IDENTITY_LEN = 64;
    const int ATLAS_PSK_LEN      = 64;
} // anonymous namespace

AtlasIdentity &AtlasIdentity::getInstance()
{
    static AtlasIdentity instance;

    return instance;
}

bool AtlasIdentity::generateIdentity()
{
    std::fstream fd;
    uuid_t uuid;
    char identity[ATLAS_IDENTITY_LEN + 1]  = { 0 };
    
    ATLAS_LOGGER_DEBUG("Generate identity...");

    fd.open(ATLAS_IDENTITY_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Error when opening the identity file");
        return false;
    }
    fs::path file = ATLAS_IDENTITY_FILE;
    fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);

   /* Generate identity */
    uuid_generate(uuid);
    uuid_unparse_lower(uuid, identity);
    identity_.assign(identity);
    fd << identity;
    fd.close();

    ATLAS_LOGGER_DEBUG("Identity was generated");

    return true;
}
void AtlasIdentity::to_base64(const uint8_t *in, size_t in_len, char *out, size_t out_len)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;
    size_t i;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, in, in_len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    memcpy(out, bptr->data,
           out_len > bptr->length ? bptr->length : out_len);

    /* Sanitize base64 psk: replace '/' and '+' characters, as these have a role in
       the MQTT topic (PSK is used as MQTT topic)*/
    for (i = 0; i < out_len; i++)
        if (out[i] == '+' || out[i] == '/')
            out[i] = 'A';

    BIO_free_all(b64);
}

bool AtlasIdentity::generatePsk()
{
    std::fstream fd;
    uint8_t rand_bytes[ATLAS_PSK_LEN/2];
    char psk[ATLAS_PSK_LEN + 1] = { 0 };
    
    ATLAS_LOGGER_DEBUG("Generate PSK...");

    fd.open(ATLAS_PSK_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Error when opening the PSK file");
        return false;
    }
    fs::path file = ATLAS_PSK_FILE;
    fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);

    /* Generate PSK */
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) != 1) {
        fd.close();
	return false;
    }

    to_base64(rand_bytes, sizeof(rand_bytes), psk, sizeof(psk) - 1);
    psk_.assign(psk);
    fd << psk;
    fd.close();

    return true;
}

bool AtlasIdentity::initIdentity()
{
    std::ifstream fd;
    int len;

    ATLAS_LOGGER_DEBUG("Identity init...");

    /* Get identity */
    fd.open(ATLAS_IDENTITY_FILE);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Identity file does not exist, generating an identity...");
        if (!generateIdentity())
            goto ERR;
    } else {
        std::stringstream strStream;
        strStream << fd.rdbuf();
        identity_ = strStream.str();
        len = fd.tellg();
        fd.close();
        if (len <= 0) {
            ATLAS_LOGGER_ERROR("Error encountered when reading from the identity file");
            goto ERR;
        }
    }

    /* Get PSK */
    fd.open(ATLAS_PSK_FILE);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("PSK file does not exist, generating a PSK...");
        if (!generatePsk())
            goto ERR;
    } else {
        std::stringstream strStream;
        strStream << fd.rdbuf();
        psk_ = strStream.str();
        len = fd.tellg();
        fd.close();
        if (len <= 0) {
            ATLAS_LOGGER_ERROR("Error encountered when reading from the PSK file");
            goto ERR;
        }
    }

    return true;

ERR:
    std::remove(ATLAS_IDENTITY_FILE.c_str());
    std::remove(ATLAS_PSK_FILE.c_str());

    return false;
}

} // namespace atlas
