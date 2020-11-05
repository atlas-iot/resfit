#ifndef __ATLAS_ALARM_H__
#define __ATLAS_ALARM_H__

#include <stdint.h>
#include <functional>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_set>

namespace atlas {

class AtlasAlarm
{

public:

    /**
     * @brief Ctor for alarm
     * @param[in] Alarm name
     * @param[in] periodMs Timer period in ms
     * @param[in] once Indicates if the timer should run only once
     * @param[in] callback Timer callback
     * @return none
     */
    AtlasAlarm(const std::string &name, uint32_t periodMs, bool once, std::function<void()> callback);

    /**
     * @brief Dtor for alarm. It cancels any active timers.
     * @return none
     */
    ~AtlasAlarm();

    /**
     * @brief Start an alarm
     * @return none
     */
    void start();

    /**
     * @brief Cancel a started alarm
     * @return none
     */
    void cancel();

private:
    /**
     * @brief Internal timer handler
     * @param[in] ec Error code
     * @param[in] alarmName Alarm name
     * @param[in] timerPtr Timer pointer
     * @return none
     */
    void timerHandler(const boost::system::error_code& ec, std::string alarmName,
                      std::weak_ptr<boost::asio::deadline_timer> timerPtr);

    /* Alarm name */
    std::string name_;

    /* Timer period in ms */
    uint32_t periodMs_;

    /* Indicates if the timer should run only once */
    bool once_;

    /* High layer application callback */
    std::function<void()> callback_;

    /* Timer object */
    std::shared_ptr<boost::asio::deadline_timer> timer_;

    /* Indicates if the alarm is started */
    bool started_;
};

} // namespace atlas

#endif /* __ATLAS_ALARM_H__ */
