#ifndef __ATLAS_SCHEDULER_H__
#define __ATLAS_SCHEDULER_H__

#include <unordered_map>
#include <utility>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasSchedulerFdEntry.h"

namespace atlas {

typedef std::pair<boost::asio::local::stream_protocol::acceptor*, boost::asio::local::stream_protocol::socket*> unix_sock_info_t;

class AtlasScheduler {

public:

    /**
     * @brief Get singleton instance for scheduler
     * @return Scheduler instance
     */
    static AtlasScheduler &getInstance();

    /**
    * @brief Get a reference to the internal service
    * @return Reference to the internal service
    */
    inline boost::asio::io_service& getService() { return ioService_; }

    /*
    * @brief Run scheduler main loop
    * @return none
    */
    void run();
    
    /*
    * @brief Add file descriptor entry to scheduler
    * @param[in] fd File descriptor
    * @param[in] cb callback
    * @return none
    */
    void addFdEntry(int fd, std::function<void()> cb);

    /**
    * @brief Delete file descriptor entry from scheduler
    * @param[in] fd File descriptor
    * @return none
    */ 
    void delFdEntry(int fd);

private:
    /**
     * @brief Ctor for scheduler
     * @return none
     */
    AtlasScheduler();

    /* Map to store the file descriptor entries */
    std::unordered_map<int, std::unique_ptr<AtlasSchedulerFdEntry>> entries_;
    
    /* Boost io_service */
    boost::asio::io_service ioService_;
    
    /* Boost io_service work (main loop) */
    boost::asio::io_service::work work_;
};

} // namespace atlas

#endif /* __ATLAS_SCHEDULER_H__ */
