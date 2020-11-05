#ifndef __ATLAS_UTILS_H__
#define __ATLAS_UTILS_H__

#include <string>
#include <vector>

namespace atlas {

const std::string ATLAS_PUB_SUB_AGENT_SOCK = "/tmp/atlas_pub_sub_agent";

namespace base64
{

/**
 * @brief Base64 encode
 * @param[in] input Encoding input
 * @return Base64 encoded string
 */
std::string encode(const std::vector<uint8_t>& input);

/**
 * @brief Base64 decode
 * @param[in] input Decoding input
 * @return Base64 decoded array
 */
std::vector<uint8_t> decode(const std::string& input);

} // namespace base64

namespace crypto
{

/**
 * @brief Execute HMAC-SHA512
 * @param[in] key Secret key
 * @param[in] payload Payload
 * @return HMAC-SHA512 output
 */
std::vector<uint8_t> hmacSHA512(const std::vector<uint8_t> &key, const std::vector<uint8_t> &payload);

} // namespace crypto

} // namespace atlas

#endif /* __ATLAS_UTILS_H__ */
