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
        logger.debugArray("All Duty: ", globals.allMotorVals, totalMotors);
    }

    void printPCADuty() {
        if (!conf.motor_map_i2c_num) {
            return;
        }

        logger.debugArray("I2C Duty: ", globals.pcaMotorVals, conf.motor_map_i2c_num);
    }

    void printLEDCDuty() {
        if (!Haptics::conf.motor_map_ledc_num) return;

        logger.debugArray("LEDC Duty: ", globals.ledcMotorVals, conf.motor_map_ledc_num);
    }


    void printAllDuty() {
        printMotorDuty();
        printPCADuty();
        printLEDCDuty();
    }

}
}
