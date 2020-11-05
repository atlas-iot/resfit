#include <iostream>
#include <boost/bind.hpp>
#include "AtlasHttpServer.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

namespace {

const std::string INDEX = "/";
const std::string INDEX_PAYLOAD = "ATLAS Gateway internal HTTP2 server!";
const std::string ANY_ADDRESS = "0.0.0.0";

} // anonymous namespace

AtlasHttpServer& AtlasHttpServer::getInstance()
{
    static AtlasHttpServer server;

    return server;
}

AtlasHttpServer::AtlasHttpServer() : tls_(boost::asio::ssl::context::tlsv13)
{
    /* Configure the HTTP server with only 1 thread (an important assumption) */
    server_.num_threads(1);
}

bool AtlasHttpServer::start(const std::string &certFile, const std::string &privKeyFile, int port)
{
    static boost::system::error_code ec;

    tls_.use_private_key_file(privKeyFile, boost::asio::ssl::context::pem);
    tls_.use_certificate_chain_file(certFile);

    configure_tls_context_easy(ec, tls_);

    /* Install default index handler */
    server_.handle(INDEX, [](const request &req, const response &res) {
        res.write_head(200);
        res.end(INDEX_PAYLOAD);
    });

    if (server_.listen_and_serve(ec, tls_, ANY_ADDRESS, std::to_string(port), true)) {
    	ATLAS_LOGGER_ERROR("Error when starting HTTP2 server (listen and server)");
        return false;
    }

    return true;
}

void AtlasHttpServer::stop()
{
    /* Stop HTTP2 server */
    server_.join();
    server_.stop();
}

void AtlasHttpServer::handleRequest(const request &req, const response &res)
{
    /* If path is not registered */
    if (callbacks_.find(req.uri().path) == callbacks_.end()) {
        ATLAS_LOGGER_ERROR("Path " + req.uri().path + " is not found");
        res.write_head(404);
        res.end();
        return;
    }

    AtlasHttpCallback &callback = callbacks_[req.uri().path];

    AtlasHttpMethod method;

    if (req.method() == "GET")
        method = AtlasHttpMethod::ATLAS_HTTP_GET;
    else if (req.method() == "POST")
        method = AtlasHttpMethod::ATLAS_HTTP_POST;
    else if (req.method() == "PUT")
        method = AtlasHttpMethod::ATLAS_HTTP_PUT;
    else if (req.method() == "DELETE")
        method = AtlasHttpMethod::ATLAS_HTTP_DEL;
    else {
        ATLAS_LOGGER_ERROR("Unsupported HTTP method " + req.method() + " for path: " + req.uri().path);
	    res.write_head(404);
        res.end();
        return;
    }

    if (method != callback.getMethod()) {
        ATLAS_LOGGER_ERROR("HTTP method " + req.method() + " unavailable for path: " + req.uri().path);
        res.write_head(404);
        res.end();
        return;
    }

    if (!callback.getHandler()) {
        ATLAS_LOGGER_ERROR("Handler method unavailbale for path: " + req.uri().path);
        res.write_head(404);
        res.end();
        return;
    }

    req.on_data([method, callback, &res] (const uint8_t *data, std::size_t len) { 
        if (!data || !len)
            return;

        std::string reqPayload((const char*) data, len);
        
        /* Execute high layer application callback (post the operation on the main scheduler thread) */
        AtlasScheduler::getInstance().getService().post([method, callback, reqPayload, &res] () {
            ATLAS_LOGGER_INFO("Execute HTTP callback handler for path: " + callback.getPath());
            AtlasHttpResponse httpResp = callback.getHandler()(method, callback.getPath(), reqPayload);

            /* Post response back on the HTTP server thread */
            AtlasHttpServer::getInstance().getService().post([httpResp, &res]() {
                /* Set response status */
                res.write_head(httpResp.getStatusCode());
                /* Set response payload */
                if (httpResp.getPayload())
                    res.end(*httpResp.getPayload());
                else
                    res.end();
            });
        });
    });
}

bool AtlasHttpServer::addCallback(const AtlasHttpCallback &httpCallback)
{
    bool ret = server_.handle(httpCallback.getPath(), boost::bind(&AtlasHttpServer::handleRequest, this, _1, _2));

    if (ret)
        callbacks_[httpCallback.getPath()] = httpCallback;
    
    return ret;
}

AtlasHttpServer::~AtlasHttpServer()
{
}

} // namespace atlas
