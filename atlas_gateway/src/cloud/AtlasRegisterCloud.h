#ifndef __ATLAS_REGISTER_CLOUD_H__
#define __ATLAS_REGISTER_CLOUD_H__

#include "../mqtt_client/IAtlasMqttState.h"
#include "../alarm/AtlasAlarm.h"

namespace atlas {

class AtlasRegisterCloud: public IAtlasMqttState
{
public:
    /**
    * @brief Get instance for cloud register module
    * @return Cloud register module instance
    */
    static AtlasRegisterCloud& getInstance();

    /**
    * @brief MQTT broker connect callback
    * @return none
    */
    void onConnect() override;

    /**
    * @brief MQTT broker disconnect callback
    * @return none
    */
    void onDisconnect() override;

    /**
    * @brief Start cloud register module
    * @return none
    */
    void start();

    /**
    * @brief Stop cloud register module
    * @return none
    */
    void stop();

    /**
    * @brief Indicates if the cloud module is registered
    * @return True if the cloud module is registered, false otherwise
    */
    bool isRegistered() const { return registered_; }

    /**
    * @brief Send register command to cloud back-end
    * @return none
    */
    void sendRegisterCmd();

    AtlasRegisterCloud(const AtlasRegisterCloud &) = delete;
    AtlasRegisterCloud& operator=(const AtlasRegisterCloud &) = delete;

private:
    /**
    * @brief Default ctor for cloud register module
    * @return none
    */
    AtlasRegisterCloud();

    /**
    * @brief Keep-alive alarm callback
    * @return none
    */
    void keepaliveAlarmCb();
    
    /**
    * @brief Send register command to cloud back-end
    * @return none
    */
    void sendKeepaliveCmd();

    /* Indicates if the cloud module is registered */
    bool registered_;

    /* Keep-alive alarm */
    AtlasAlarm kaAlarm_;
};

} // namespace atlas

#endif /* __ATLAS_KEEP_ALIVE_CLOUD_H__ */
