#include <boost/bind.hpp>
#include "AtlasAlarm.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

void AtlasAlarm::timerHandler(const boost::system::error_code& ec, std::string alarmName,
                              std::weak_ptr<boost::asio::deadline_timer> timerPtr)
{
    ATLAS_LOGGER_DEBUG("Timer handler executed for alarm with name " + alarmName);

    /* If an error occured */
    if (ec) {
        if (ec == boost::asio::error::operation_aborted)
            ATLAS_LOGGER_DEBUG("Timer aborted for alarm with name " + alarmName);
        else
            ATLAS_LOGGER_ERROR("Timer handler for alarm with name " + alarmName + " called with error message: " + ec.message());

        return;
    }

    auto timer = timerPtr.lock();

    if (!timer) {
        ATLAS_LOGGER_DEBUG("Timer invalid for alarm with name " + alarmName);
        return;
    }

    /* Start timer again */
    if (!once_) {
        timer->expires_at(timer_->expires_at() + boost::posix_time::millisec(this->periodMs_));
        timer->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1, alarmName, timerPtr));
    } else
        cancel();

    if (callback_)
        callback_();
}

AtlasAlarm::AtlasAlarm(const std::string &name, uint32_t periodMs,
                       bool once, std::function<void()> callback) : timer_(nullptr), started_(false)
{
    ATLAS_LOGGER_DEBUG("Alarm was created");

    name_ = name;
    periodMs_ = periodMs;
    once_ = once;
    callback_ = callback;
}

void AtlasAlarm::start()
{
    /* If timer is already started, there is nothing to do */
    if (started_)
        return;

    ATLAS_LOGGER_DEBUG("Start alarm with name: " + name_);

    timer_ = std::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(AtlasScheduler::getInstance().getService(),
                                                          boost::posix_time::millisec(this->periodMs_)));

    /* Start timer now */
    timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1, name_,
                       std::weak_ptr<boost::asio::deadline_timer>(timer_)));

    started_ = true;

    ATLAS_LOGGER_DEBUG("Alarm was started");
}

void AtlasAlarm::cancel()
{
    if (started_) {
        ATLAS_LOGGER_DEBUG("Alarm with name " + name_ + " was cancelled");
        timer_->cancel();
        timer_.reset();
        started_ = false;
    }
}

AtlasAlarm::~AtlasAlarm()
{
    cancel();
}

} // namespace atlas
