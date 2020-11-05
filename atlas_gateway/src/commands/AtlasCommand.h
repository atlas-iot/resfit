#ifndef __ATLAS_COMMAND_H__
#define __ATLAS_COMMAND_H__

#include <stdint.h>

namespace atlas {

/* Command header length (type + length) */
const int ATLAS_CMD_HEADER_LEN = 4;

class AtlasCommand
{

public:
    /**
    * @brief Ctor for command
    * @param[in] type Command type
    * @param[in] len Command length
    * @param[in] val Command value
    * @return none
    */
    AtlasCommand(uint16_t type, uint16_t len, const uint8_t *val) : type_(type), len_(len), val_(val) {}
    
    /**
    * @brief Getter for command type
    * @return command type
    */
    uint16_t getType() const { return type_; }
    
    /**
    * @brief Getter for command length
    * @return command length
    */
    uint16_t getLen() const { return len_; }
    
    /**
    * @brief Getter for command value
    * @return command value
    */
    const uint8_t *getVal() const { return val_; }
private:
    /* Command type */
    uint16_t type_;
    /* Command length */
    uint16_t len_;
    /* Command value */
    const uint8_t *val_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_H__ */
