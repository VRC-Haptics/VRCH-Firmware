#include "serial.h"
#include "config/config.h"  // Provides Haptics::conf and Haptics::defaultConfig
#include "globals.h"        // Provides Haptics::globals
#include "logging/Logger.h" // For logger calls
#include "board_defines.h"  // Provides board-specific defines
#include "PWM/LEDC/ledc.h"
#include "wifi/callbacks.h"
#include <Arduino.h>

// Helper function to trim leading/trailing whitespace from a String.
static String trim(const String &str) {
  unsigned int start = 0;
  while (start < str.length() && isspace(str.charAt(start)))
    start++;
  unsigned int end = str.length() - 1;
  while (end >= 0 && isspace(str.charAt(end)))
    end--;
  return (end >= start) ? str.substring(start, end + 1) : "";
}



namespace Haptics {
  namespace SerialComm {

    Logging::Logger logger("Serial");

    // Main tick() function called from loop().
    void tick() {
      if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input = trim(input);
        if (input.length() == 0)
          return;
      
        Haptics::globals.commandToProcess = input;
        Haptics::globals.processSerCommand = true;
        logger.debug("New input: ", input);
      }
    }

  } // namespace SerialComm
} // namespace Haptics
