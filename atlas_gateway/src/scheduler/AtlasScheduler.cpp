#include "AtlasScheduler.h"
#include <iostream>


namespace atlas {

AtlasScheduler &AtlasScheduler::getInstance()
{
    static AtlasScheduler scheduler;

    return scheduler;
}

AtlasScheduler::AtlasScheduler() : work_(ioService_) {}

void AtlasScheduler::addFdEntry(int fd, std::function<void()> cb)
{
    std::unique_ptr<AtlasSchedulerFdEntry> entry(new AtlasSchedulerFdEntry(ioService_, fd, cb));
    
    /*Add scheduler entry to list */
    entries_[fd] = std::move(entry);
}

void AtlasScheduler::delFdEntry(int fd)
{
    entries_.erase(fd);
}

void AtlasScheduler::run()
{
    ioService_.run();
}

} // namespace atlas
