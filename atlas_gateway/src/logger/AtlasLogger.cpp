#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <fstream>
#include "AtlasLogger.h"

#define ATLAS_GATEWAY_LOG_FILE "atlas_gateway.log"

namespace atlas {

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", boost::log::trivial::severity_level)

boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> logger;

void initLog()
{
    boost::shared_ptr< logging::core > core = logging::core::get();

    // add attributes
    logger.add_attribute("TimeStamp", attrs::local_clock());

    boost::shared_ptr< logging::sinks::text_file_backend > backend =
        boost::make_shared< logging::sinks::text_file_backend >(
            logging::keywords::file_name = ATLAS_GATEWAY_LOG_FILE,
            logging::keywords::open_mode = std::ios_base::out | std::ios_base::app
        );

    // Enable auto-flushing after each log record written
    backend->auto_flush(true);

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef logging::sinks::synchronous_sink< logging::sinks::text_file_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));

     // specify the format of the log message
    boost::log::formatter formatter = expr::stream
      << expr::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S.%f") << " "
      << "[" << boost::log::trivial::severity << "]"
      << " - " << expr::smessage;

    sink->set_formatter(formatter);

    core->add_sink(sink);
}

} // namespace atlas

