#include "AtlasUtils.h"

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <openssl/hmac.h>

using it_binary_t = boost::archive::iterators::transform_width<
    boost::archive::iterators::binary_from_base64<
        boost::archive::iterators::remove_whitespace<
            std::string::const_iterator
        >
    >,8,6
>;

using it_base64_t = boost::archive::iterators::insert_linebreaks<
    boost::archive::iterators::base64_from_binary<
        boost::archive::iterators::transform_width<
            std::vector<uint8_t>::const_iterator,6,8
        >
    >,72
>;

namespace atlas
{

namespace base64
{

std::string encode(const std::vector<uint8_t>& input)
{
    const auto pad = (3 - input.size() % 3) % 3;
    auto ret = std::string(it_base64_t(input.begin()),it_base64_t(input.end()));
    ret.append(pad,'=');

    return ret;
}

std::vector<uint8_t> decode(const std::string& input)
{
    auto base64 = input;
    const auto pad = count(base64.begin(),base64.end(),'=');
    std::replace(base64.begin(),base64.end(),'=','A');
    auto ret = std::vector<uint8_t>(it_binary_t(base64.begin()),it_binary_t(base64.end()));
    ret.erase(ret.end()-pad,ret.end());
    
    return ret;
}

} // namespace base64

namespace crypto
{

namespace
{
    const int ATLAS_HMAC_SHA512_SIZE_BYTES = 64;
} // anonymous namespace

std::vector<uint8_t> hmacSHA512(const std::vector<uint8_t> &key, const std::vector<uint8_t> &payload)
{
    uint8_t hash[ATLAS_HMAC_SHA512_SIZE_BYTES] = { 0 };
    uint32_t len = 0;

    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key.data(), key.size(), EVP_sha512(), NULL);
    HMAC_Update(ctx, payload.data(), payload.size());
    HMAC_Final(ctx, hash, &len);
    HMAC_CTX_free(ctx);

    return std::vector<uint8_t>(hash, hash + sizeof(hash));
}

} // namespace crypto

} // namespace atlas
