#include "AtlasSchedulerFdEntry.h"
#include <iostream>
#include <boost/bind.hpp>

namespace atlas {

void AtlasSchedulerFdEntry::handleFdEvent(boost::system::error_code ec, size_t xferLen)
{
    if (!ec) {
        /* Schedule another read */
        desc_.async_read_some(boost::asio::null_buffers(),
                              boost::bind(&AtlasSchedulerFdEntry::handleFdEvent, this, _1, _2));

        this->callback_();
    }
} 

AtlasSchedulerFdEntry::AtlasSchedulerFdEntry(boost::asio::io_service &ioService,
                                             int fd, std::function<void()> cb) : desc_(ioService)
{
    this->fd_ = fd;
    this->callback_ = cb;

    desc_.assign(fd);

    desc_.async_read_some(boost::asio::null_buffers(),
                          boost::bind(&AtlasSchedulerFdEntry::handleFdEvent, this, _1, _2));
}

AtlasSchedulerFdEntry::~AtlasSchedulerFdEntry()
{
    this->desc_.cancel();
    this->desc_.release();
    this->desc_.close();
}

} // namespace atlas
