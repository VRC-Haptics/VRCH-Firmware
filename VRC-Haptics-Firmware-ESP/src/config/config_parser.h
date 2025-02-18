#include <Arduino.h>

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

namespace Haptics { 
    /// @brief Takes an input string containing a command and returns the response to be retransmitted
    /// @param input The string that should be parsed
    /// @return The response or feedback containing either confirmation or the requested data
    String parseInput(const String &input);
}



#endif // CONFIG_PARSER_H