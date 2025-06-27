#include <Arduino.h>

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "config.h"

namespace Haptics {
namespace Conf { 
namespace Parser {
    /// @brief Takes an input string containing a command and returns the response to be retransmitted
    /// @param input The string that should be parsed
    /// @return The response or feedback containing either confirmation or the requested data
    String parseInput(const String &input);

    String getArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field);
    bool setArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field, const String& input);
}
}
}
#endif // CONFIG_PARSER_H