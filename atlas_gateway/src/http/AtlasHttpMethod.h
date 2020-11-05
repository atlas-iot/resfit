#ifndef __ATLAS_HTTP_METHOD_H__
#define __ATLAS_HTTP_METHOD_H__

namespace atlas {

enum AtlasHttpMethod {
    /* HTTP GET method */
    ATLAS_HTTP_GET = 0,
    /* HTTP POST method */
    ATLAS_HTTP_POST,
    /* HTTP PUT method */
    ATLAS_HTTP_PUT,
    /* HTTP DEL method */
    ATLAS_HTTP_DEL,
};

} // namespace atlas

#endif /* __ATLAS_HTTP_METHOD_H__ */
