#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>

namespace Haptics {
  namespace SerialComm {
    // Call this function every loop iteration to check for new serial input.
    void tick();
  }
}

#endif // SERIAL_H