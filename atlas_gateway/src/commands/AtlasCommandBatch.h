#ifndef __ATLAS_COMMAND_BATCH_H__
#define __ATLAS_COMMAND_BATCH_H__

#include <vector>
#include <utility>
#include "AtlasCommand.h"

namespace atlas {

class AtlasCommandBatch
{

public:
    /**
    * @brief Ctor for command batch
    * @return none
    */
    AtlasCommandBatch() : parsedCmdBuf_(nullptr), addedCmdBuf_(nullptr) {}

    /**
    * @brief Add command
    * @param[in] cmd Command
    * @return none
    */
    void addCommand(const AtlasCommand &cmd);

    /**
    * @brief Get a serialized array of the previously added commands
    * @return A pair consisting in a buffer and a buffer length
    */
    std::pair<const uint8_t*, size_t> getSerializedAddedCommands();

    /**
    * @brief Provide the command batch a raw command byte array
    * @param[in] cmd Raw command byte array
    * @param[in] cmdLen Raw command byte array length
    * @return none
    */
    void setRawCommands(const uint8_t *cmd, size_t cmdLen);

    /**
    * @brief Get parsed commands
    * @return Parsed commands
    */
    std::vector<AtlasCommand> getParsedCommands() const { return parsedCmd_; }

    /**
    * @brief Dtor for command batch
    * @return none
    */
    ~AtlasCommandBatch();
private:
    /**
    * @brief Parse raw commands
    * @param[in] cmdLen Raw commands buffer length
    * @return none
    */
    void parseCommands(size_t cmdLen);

    /* Raw commands byte array */
    uint8_t *parsedCmdBuf_;
    
    /* Added commands byte array */
    uint8_t *addedCmdBuf_;
    
    /* Parsed commands list */
    std::vector<AtlasCommand> parsedCmd_;

    /* Added commands list */
    std::vector<AtlasCommand> addedCmd_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_BATCH_H__ */
