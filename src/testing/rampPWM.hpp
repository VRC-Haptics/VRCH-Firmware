#include "Arduino.h"
#include "PWM/PCA/pca.h"
#include "globals.h"

namespace Haptics {
namespace PwmUtils {
  void setMotorArray(uint16_t state) {
    const uint16_t totalMotors =  Haptics::Conf::conf.motor_map_i2c_num+ Haptics::Conf::conf.motor_map_ledc_num;
    for (int i = 0; i < totalMotors; i++) {
      Haptics::globals.allMotorVals[i] = state;
    }
  }

  /// @brief Ramp PCA pwm up and down for testing
  void rampTesting() {
    Serial.println("Entering Motor Ramp");
    for (uint16_t state = 0; state < 1024; state++) {
      PCA::setAllPcaDuty(state*4, &Haptics::Conf::conf);
      delay(1);
    }
    for (uint16_t state = 1024; state > 0; state--) {
      PCA::setAllPcaDuty(state*4, &Haptics::Conf::conf);
      delay(1);
    }
  }

}
}
