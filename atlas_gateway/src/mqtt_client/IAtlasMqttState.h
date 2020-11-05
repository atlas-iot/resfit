#ifndef __IATLAS_MQTT_STATE_H__
#define __IATLAS_MQTT_STATE_H__

namespace atlas {

class IAtlasMqttState
{
public:
    /**
    * @brief MQTT broker connect callback
    * @return none
    */
    virtual void onConnect() = 0;

     /**
    * @brief MQTT broker disconnect callback
    * @return none
    */
    virtual void onDisconnect() = 0;
};

} // namespace atlas

#endif /* __IATLAS_MQTT_STATE_H__ */
