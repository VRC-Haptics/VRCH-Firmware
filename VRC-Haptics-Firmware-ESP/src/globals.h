#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "software_defines.h"

namespace Haptics {
    // Volatile, non-static, user-denied variables
    struct Globals {
        uint8_t ledcMotorVals[MAX_LEDC_MOTORS];
        uint16_t pcaMotorVals[MAX_I2C_MOTORS];
        uint16_t allMotorVals[MAX_LEDC_MOTORS + MAX_I2C_MOTORS];
        bool updatedMotors;
        bool reinitLEDC;
        bool processOscCommand; // moves the heavy commands out of ISR time
        bool processSerCommand;
        String commandToProcess;
    };

    inline Globals initGlobals() {
        Globals g = {};
        g.reinitLEDC = false;
        g.updatedMotors = false;
        g.processOscCommand = false;
        g.processSerCommand = false;
        g.commandToProcess = "";
        return g;
    }

    // Define a structure to store timing data
    struct TimingData {
        uint32_t digitalWriteCycles;
        uint32_t loopCycles;
    };

    inline TimingData profiler = {0, 0};

    // Declare a global instance of Globals.
    inline Globals globals = initGlobals();
} // namespace Haptics

#endif // GLOBALS_H