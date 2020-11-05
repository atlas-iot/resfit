#include <random>
#include <boost/bind.hpp>
#include <json/json.h>
#include "AtlasClaim.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";

/* JSON fields from the incoming claim request */
const std::string ATLAS_CLAIM_SHORT_CODE_JSON_KEY = "shortCode";
const std::string ATLAS_CLAIM_SECRET_KEY_JSON_KEY = "secretKey";
const std::string ATLAS_CLAIM_OWNER_IDENTITY_JSON_KEY = "ownerIdentity";

const std::string ATLAS_CLAIM_SHORT_CODE_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
int ATLAS_CLAIM_SHORT_CODE_LEN = 6;
int ATLAS_CLAIM_ALARM_PERIOD_MS = 180000; // change short code every 3 minutes

/* Error messages */
const std::string ATLAS_CLAIM_ERR_ALREADY_CLAIMED = "Gateway is already claimed by an owner!";
const std::string ATLAS_CLAIM_ERR_EMPTY_PAYLOAD = "Rejecting claim request because payload is empty";
const std::string ATLAS_CLAIM_ERR_SHORT_CODE = "Rejecting claim request because the short code is invalid!";
const std::string ATLAS_CLAIM_ERR_EMPTY_SECRET_KEY = "Empty claim secret key";
const std::string ATLAS_CLAIM_ERR_EMPTY_IDENTITY = "Empty owner identity";
const std::string ATLAS_CLAIM_ERR_DATABASE = "Cannot save owner information into the database";

/* JSON response (on success) */
const std::string ATLAS_CLAIM_RESPONSE_IDENTITY_JSON_KEY = "identity";

} // anonymous namespace

AtlasClaim& AtlasClaim::getInstance()
{
    static AtlasClaim instance;
    return instance;
}

AtlasClaim::AtlasClaim() : shortCodeAlarm_("AtlasClaim", ATLAS_CLAIM_ALARM_PERIOD_MS, false,
                                           boost::bind(&AtlasClaim::alarmCallback, this)),
                           claimCallback_(AtlasHttpMethod::ATLAS_HTTP_POST, ATLAS_CLAIM_REQUEST_PATH,
                                          boost::bind(&AtlasClaim::handleClaimReq, this, _1, _2, _3)) { }

bool AtlasClaim::start()
{
    if (!AtlasDeviceManager::getInstance().getGateway().isClaimed()) {
        ATLAS_LOGGER_ERROR("Cannot get owner (claim) information from database");
        
        /* Generate short code */
        shortCode_ = generateShortCode();
        ATLAS_LOGGER_INFO("Claim protocol short code is: " + shortCode_);

        /* Start short code alarm */
        shortCodeAlarm_.start();
    } else
        ATLAS_LOGGER_INFO("Gateway is claimed by owner with identity: " +
                          AtlasDeviceManager::getInstance().getGateway().getOwnerIdentity());
    
    /* Register callback to HTTP server */
    return AtlasHttpServer::getInstance().addCallback(claimCallback_);
}

std::string AtlasClaim::generateShortCode()
{
    std::string str(ATLAS_CLAIM_SHORT_CODE_ALPHABET);
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, ATLAS_CLAIM_SHORT_CODE_LEN);
}

void AtlasClaim::alarmCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for claiming protocol");

    /* Generate short code */
    shortCode_ = generateShortCode();
    ATLAS_LOGGER_INFO("Claim protocol short code is: " + shortCode_);
}

AtlasHttpResponse AtlasClaim::handleClaimReq(AtlasHttpMethod method, const std::string &path,
                                             const std::string &payload)
{
    Json::Reader reader;
    Json::Value obj;

    ATLAS_LOGGER_DEBUG("Handle claim request on path: " + path);

    if (AtlasDeviceManager::getInstance().getGateway().isClaimed()) {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_ALREADY_CLAIMED);
        return AtlasHttpResponse(401, ATLAS_CLAIM_ERR_ALREADY_CLAIMED);
    }

    if (payload == "") {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_EMPTY_PAYLOAD);
        return AtlasHttpResponse(401, ATLAS_CLAIM_ERR_EMPTY_PAYLOAD);
    }

    reader.parse(payload, obj);

    if (obj[ATLAS_CLAIM_SHORT_CODE_JSON_KEY].asString() != shortCode_) {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_SHORT_CODE);
        return AtlasHttpResponse(401, ATLAS_CLAIM_ERR_SHORT_CODE);
    } 

    if (obj[ATLAS_CLAIM_SECRET_KEY_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_EMPTY_SECRET_KEY);
        return AtlasHttpResponse(500, ATLAS_CLAIM_ERR_EMPTY_SECRET_KEY);	
    }

    if (obj[ATLAS_CLAIM_OWNER_IDENTITY_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_EMPTY_IDENTITY);
        return AtlasHttpResponse(500, ATLAS_CLAIM_ERR_EMPTY_IDENTITY);	
    }

    /* Claim gateway device */
    bool result = AtlasDeviceManager::getInstance().getGateway().claimGateway(obj[ATLAS_CLAIM_OWNER_IDENTITY_JSON_KEY].asString(),
                                                                              obj[ATLAS_CLAIM_SECRET_KEY_JSON_KEY].asString());

    if(!result) {
        ATLAS_LOGGER_ERROR(ATLAS_CLAIM_ERR_DATABASE);
        return AtlasHttpResponse(500, ATLAS_CLAIM_ERR_DATABASE);
    }

    ATLAS_LOGGER_INFO("Gateway is successfully claimed!");

    shortCodeAlarm_.cancel();

    Json::FastWriter fastWriter;
    Json::Value cmd;
    cmd[ATLAS_CLAIM_RESPONSE_IDENTITY_JSON_KEY] = AtlasIdentity::getInstance().getIdentity();
    std::string claimResp = fastWriter.write(cmd);

    return AtlasHttpResponse(200, claimResp);
}

} // namespace atlas
