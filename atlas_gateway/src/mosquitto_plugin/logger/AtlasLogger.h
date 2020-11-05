#ifndef __ATLAS_LOGGER_H__
#define __ATLAS_LOGGER_H__

#include <syslog.h>
#include <mosquitto.h>

#define ATLAS_LOGGER_DEBUG(...) syslog(LOG_DEBUG, __VA_ARGS__)
#define ATLAS_LOGGER_INFO(...) syslog(LOG_INFO, __VA_ARGS__)
#define ATLAS_LOGGER_ERROR(...) syslog(LOG_ERR, __VA_ARGS__)

namespace atlas {

/**
* @brief Init logger
* @return none
*/
void atlasLoggerInit();

/**
* @brief Close logger
* @return none
*/
void atlasLoggerClose();

} // namespace atlas

#endif /* __ATLAS_MOSQUITTO_LOG_H__ */
