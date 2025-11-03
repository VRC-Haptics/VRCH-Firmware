#include "pca.h"

namespace Haptics {
namespace PCA {

Adafruit_PWMServoDriver pcaModule1 = Adafruit_PWMServoDriver(PCA_1, Wire);//, Wire);
Adafruit_PWMServoDriver pcaModule2 = Adafruit_PWMServoDriver(PCA_2, Wire);//, Wire);

bool firstPCAConnected = false;
bool secondPCAConnected = false;

uint16_t debounceBuffer[MAX_I2C_MOTORS+1];
Logging::Logger logger("I2C");

/// @brief Start pca module communication
void start(Haptics::Conf::Config *conf) {
  Wire.begin(conf->i2c_sda, conf->i2c_scl, conf->i2c_speed);

  // init modules
  if (pcaModule1.begin()) {
    logger.debug("PCA1 Connected");
    firstPCAConnected = true;
  } else {
    logger.warn("PCA1 Not Connected");
  }

  if (pcaModule2.begin()) {
    logger.debug("PCA2 Connected");
    secondPCAConnected = true;
  } else {
    logger.warn("PCA2 Not Found");
  }

  // init modules
  delay(100);
  //pcaModule1.reset();
  if (secondPCAConnected) {
    pcaModule2.reset();
  }
  
  //set frequencies
  delay(100);
  if(firstPCAConnected) {
    pcaModule1.setPWMFreq(PCA_FREQUENCY);
    pcaModule1.setOscillatorFrequency(27000000);
    logger.warn("PCA1 Prescale set to:", pcaModule1.readPrescale());
  }

  if (secondPCAConnected) {
    pcaModule2.setPWMFreq(PCA_FREQUENCY);
    pcaModule2.setOscillatorFrequency(27000000);
    logger.warn("PCA2 Prescale set to:", pcaModule2.readPrescale());
  }

  //chime
  logger.debug("Starting Chime");
  setAllPcaDuty(4095, conf);
  delay(100);
  setAllPcaDuty(0, conf);
}

/// @brief Sets PCA motors to the values from the global variables
void setPcaDuty(Globals *globals, Haptics::Conf::Config *conf) {
    for(uint8_t i = 0; i < 16; i++) {
        const uint16_t value = globals->pcaMotorVals[i];
        const uint16_t value2 = globals->pcaMotorVals[i+16];
        if (value != debounceBuffer[i] && firstPCAConnected) {
          pcaModule1.setPin(conf->motor_map_i2c[i], value >> 4);
          debounceBuffer[i] = value;
        }
        if (value2 != debounceBuffer[i+16] && secondPCAConnected) { //only send to second if it is connected
          pcaModule2.setPin(conf->motor_map_i2c[i+16]-16, value2 >> 4);// 3 HOURS JUST TO FIND THE -16..... I WANT TO DIE
          debounceBuffer[i+16] = value2;
        }
    }
}

/// @brief Sets all motors to the specified duty cycle, mapped to the PCA_MAP defined in config.h
/// @param dutyCycle The list of each motors duty cycle
void setAllPcaDuty(uint16_t duty, Haptics::Conf::Config *conf) {
  for(uint8_t i = 0; i < conf->motor_map_i2c_num; i++) {
    setPCAMotorDuty(i, duty);
  }
    
}

void setPCAMotorDuty(uint8_t motorIndex, uint16_t dutyCycle){

  if (motorIndex < 16) {
    //only push updates if they are different
    if (debounceBuffer[motorIndex] != dutyCycle && firstPCAConnected) {
      pcaModule1.setPin(motorIndex, dutyCycle);
      debounceBuffer[motorIndex] = dutyCycle;
      
    } 
  } else {
    if (debounceBuffer[motorIndex] != dutyCycle && secondPCAConnected) {
      pcaModule2.setPin(motorIndex-16, dutyCycle);
      debounceBuffer[motorIndex] = dutyCycle;
    }
  }
}

} // namespace PCA
} // namespace Haptics

