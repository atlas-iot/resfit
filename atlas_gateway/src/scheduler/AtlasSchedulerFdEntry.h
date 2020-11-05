#ifndef __ATLAS_SCHEDULER_ENTRY_H__
#define __ATLAS_SCHEDULER_ENTRY_H__

#include <functional>
#include <boost/asio.hpp>

namespace atlas {

class AtlasSchedulerFdEntry {

public:
    /*
    * @brief Ctor for scheduler entry
    * @param[in] ioService Reference to ioService
    * @param[in] fd File descriptor
    * @param[in] cb Callback
    * @return none
    */
    AtlasSchedulerFdEntry(boost::asio::io_service &ioService, int fd, std::function<void()> cb);

    /**
    * @brief Dtor for scheduler entry
    * @return none
    */
    ~AtlasSchedulerFdEntry();

private:
    /**
    * @brief Handle file descriptor event
    * @param[in] ec Error code
    * @param[in] xferLen Transfer length
    * @return none
    */
    void handleFdEvent (boost::system::error_code ec, size_t xferLen);

    /* Scheduler file descriptor */
    int fd_;

    /* Higher layer application callback*/
    std::function<void()> callback_;

    /* Boost stream descriptor */
    boost::asio::posix::stream_descriptor desc_;
};

} // namespace atlas

#endif /* __ATLAS_SCHEDULER_ENTRY_H__ */
