cmake_minimum_required(VERSION 2.8)

project(atlas_gateway)

include(FindPkgConfig)
# CoAP dependency
pkg_check_modules(COAP libcoap-2-openssl REQUIRED)

# SQLite dependency
pkg_check_modules(SQLITE sqlite3 REQUIRED)

# UUID dependency
pkg_check_modules(UUID uuid REQUIRED)

# OpenSSL dependency
find_package(OpenSSL REQUIRED)

# JSONCPP dependency
pkg_check_modules(JSONCPP jsoncpp REQUIRED)

# NGHTTP2 dependency
pkg_check_modules(NGHTTP2_ASIO libnghttp2_asio REQUIRED)

# Link with dynamic version of Boost (for logging)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(Boost 1.71.0 REQUIRED system log program_options regex)

include_directories(include)
# Add CoAP headers
include_directories(${COAP_INCLUDE_DIRS})

# Add SQLite3 headers
include_directories(${SQLITE_INCLUDE_DIRS})

# Add JSONCPP headers
include_directories(${JSONCPP_INCLUDE_DIRS})

# Add NGHTTP2 headers
include_directories(${NGHTTP2_ASIO_INCLUDE_DIRS})

# Atlas gateway files
file(GLOB ATLAS_GATEWAY_SOURCES "src/*.cpp" "src/sql/AtlasSQLite.cpp" "src/scheduler/*.cpp" "src/coap/*.cpp" "src/logger/*.cpp" "src/alarm/*.cpp" "src/register/*.cpp" "src/commands/*.cpp" "src/pubsub_agent/*.cpp" "src/device/*.cpp" "src/telemetry/*.cpp" "src/policy/*.cpp" "src/mqtt_client/*.cpp" "src/identity/*.cpp" "src/cloud/*.cpp" "src/statistics/*.cpp" "src/reputation/*.cpp" "src/reputation_impl/*cpp" "src/device/reputation_feedback/*.cpp" "src/http/*.cpp" "src/claim_approve/*.cpp" "src/utils/*.cpp")

# Atlas_gateway_cli files
file(GLOB ATLAS_GATEWAY_CLI_SOURCES "src/sql/cli/*.cpp" "src/sql/*.cpp" "src/scheduler/*.cpp" "src/coap/*.cpp" "src/logger/*.cpp" "src/alarm/*.cpp" "src/register/*.cpp" "src/commands/*.cpp" "src/pubsub_agent/*.cpp" "src/device/*.cpp" "src/telemetry/*.cpp" "src/policy/*.cpp" "src/mqtt_client/*.cpp" "src/identity/*.cpp" "src/cloud/*.cpp" "src/statistics/*.cpp" "src/reputation/*.cpp" "src/reputation_impl/*cpp" "src/device/reputation_feedback/*.cpp" "src/http/*.cpp" "src/claim_approve/*.cpp" "src/utils/*.cpp")

# Atlas_gateway_reputation_test files
file(GLOB ATLAS_GATEWAY_REPUTATION_TEST_SOURCES "src/reputation_impl/*.cpp" "src/reputation_impl/tester/*.cpp" "src/logger/*.cpp")

link_directories(${COAP_LIBRARY_DIRS})
link_directories(${SQLITE_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${ATLAS_GATEWAY_SOURCES})
add_executable(atlas_gateway_cli ${ATLAS_GATEWAY_CLI_SOURCES})
add_executable(atlas_gateway_reputation_test ${ATLAS_GATEWAY_REPUTATION_TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} paho-mqtt3as paho-mqttpp3 ${UUID_LIBRARIES} OpenSSL::Crypto OpenSSL::SSL -lstdc++fs ${JSONCPP_LIBRARIES})
target_link_libraries(atlas_gateway_cli paho-mqtt3a paho-mqttpp3 ${UUID_LIBRARIES} OpenSSL::Crypto OpenSSL::SSL -lstdc++fs ${JSONCPP_LIBRARIES})

#Add CoAP, Boost, NGHTTP2 library
target_link_libraries(${PROJECT_NAME} ${SQLITE_LIBRARIES} ${COAP_LIBRARIES} ${Boost_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${NGHTTP2_ASIO_LIBRARIES})
target_link_libraries(atlas_gateway_cli ${SQLITE_LIBRARIES} ${COAP_LIBRARIES} ${Boost_LIBRARIES} ${NGHTTP2_ASIO_LIBRARIES})
target_link_libraries(atlas_gateway_reputation_test ${Boost_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-psabi)
target_compile_options(atlas_gateway_cli PRIVATE -Wall -Wno-psabi)
target_compile_options(atlas_gateway_reputation_test PRIVATE -Wall -Wno-psabi)

# Mosquitto dependency
pkg_check_modules(MOSQUITTO libmosquitto REQUIRED)

# Atlas mosquitto plug-in files
file(GLOB ATLAS_GATEWAY_MOSQUITTO_PLUGIN_SOURCES "src/mosquitto_plugin/*.cpp" "src/mosquitto_plugin/logger/*.cpp" "src/mosquitto_plugin/filter/*.cpp" "src/commands/*.cpp")

add_library(atlas_gateway_mosquitto_plugin SHARED ${ATLAS_GATEWAY_MOSQUITTO_PLUGIN_SOURCES})
target_link_libraries(${PROJECT_NAME}_mosquitto_plugin ${MOSQUITTO_LIBRARIES} ${Boost_LIBRARIES})
target_compile_options(${PROJECT_NAME}_mosquitto_plugin PRIVATE -Wall -Wno-psabi)

install(TARGETS ${PROJECT_NAME} atlas_gateway_cli DESTINATION bin)
