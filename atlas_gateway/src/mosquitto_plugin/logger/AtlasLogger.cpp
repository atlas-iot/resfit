#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <mosquitto.h>
#include <mosquitto_plugin.h>
#include "AtlasLogger.h"

#define ATLAS_GATEWAY_MOSQ "ATLAS_GW_MOSQUITTO_PLUGIN"

namespace atlas {

void atlasLoggerInit()
{
    openlog (ATLAS_GATEWAY_MOSQ, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

void atlasLoggerClose()
{
    closelog();
}

} // namespace atlas
