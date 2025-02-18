#ifndef CALLBACKS_H 
#define CALLBACKS_H

#include <ArduinoOSCWiFi.h>
#include "globals.h"
#include "osc.h"
#include "software_defines.h"
#include "logging/Logger.h"

namespace Haptics  {
namespace Wireless {
    void printRaw();
    void updateMotorVals();
    void motorMessage_callback(const OscMessage& message);
    void printOSCMessage(const OscMessage& message);
    void commandMessageCallback(const OscMessage& msg);

} // namespace Wireless
} // namespace Haptics

#endif // CALLBACKS_H