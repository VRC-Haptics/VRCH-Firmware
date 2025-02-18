#include <Arduino.h>

#include "globals.h"
#include "logging/Logger.h"

namespace Haptics {
namespace PwmUtils {
    Logging::Logger logger("Utils");

    void printMotorDuty() {
        const uint16_t totalMotors = conf.motor_map_i2c_num+conf.motor_map_ledc_num;
        if (!totalMotors) {
            logger.debug("No configured motors");
            return;
        }

        Serial.print("Motor Duty : ");
        for (uint8_t i = 0; i < totalMotors; i++) {
            Serial.print(globals.allMotorVals[i]);
            Serial.print(", ");
        }

        Serial.println("");
    }

    void printPCADuty() {
        if (!conf.motor_map_i2c_num) {
            return;
        }

        Serial.print("PCA Duty : ");
        
        for (uint8_t i = 0; i < conf.motor_map_i2c_num; i++) {
            Serial.print(globals.pcaMotorVals[i]);
            Serial.print(", ");
        }

        Serial.println("");
    }

    void printLEDCDuty() {
        if (!Haptics::conf.motor_map_i2c_num) return;

        Serial.print("LEDC Duty : ");
        for (uint8_t i = 0; i < conf.motor_map_ledc_num; i++) {
            Serial.print(Haptics::globals.ledcMotorVals[i]);
            Serial.print(", ");
        }
        Serial.println("");
    }


    void printAllDuty() {
        printMotorDuty();
        printPCADuty();
        printLEDCDuty();
    }

}
}
