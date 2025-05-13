#include <Arduino.h>
#include "Wire.h"
#include "LittleFS.h"

// main config files
#include "globals.h"
#include "config/config.h"
#include "config/config_parser.h"
#include "logging/Logger.h"

// import modules
#include "OSC/osc.h"
#include "OSC/callbacks.h"
#include "PWM/PCA/pca.h"
#include "PWM/LEDC/ledc.h"
#include "serial/serial.h"

// testing
#include "testing/rampPWM.hpp"
#include "PWM/PWMUtils.hpp"

// Main Logger instance
Haptics::Logging::Logger logger("Main");

void setup() {
  Serial.begin(115200);

  #ifdef DEV_MODE
  //wait for serial if we are developing
  delay(700);
  #endif

  // Initialize LittleFS
  #if defined(ESP8266)
  if (!LittleFS.begin()) { // ESP8 doesnt? have the format fucntion i guess
    logger.error("LittleFS mount failed, please restart");
    return;
  }
  #else
  if (!LittleFS.begin(true)) {
    logger.error("LittleFS mount failed, please restart");
    return;
  }
  #endif

  Haptics::loadConfig();
  Haptics::initGlobals();

  Haptics::Wireless::Start(&Haptics::conf);
  Haptics::PCA::start(&Haptics::conf);
  Haptics::LEDC::start(&Haptics::conf);
}

uint32_t ticks = 0;
time_t now = 0;
time_t lastSerialPush = millis();
time_t lastWifiTick = millis();

#define TIMER_START uint32_t dwStart = ESP.getCycleCount();
#define TIMER_END Haptics::profiler.digitalWriteCycles += (ESP.getCycleCount() - dwStart);
uint32_t loopStart = 0;
uint32_t loopTotal = 0;
bool messageRecieved = false;

void loop() {
  
  if (Haptics::globals.reinitLEDC) { // prevents not defined error
    Haptics::LEDC::start(&Haptics::conf);
    logger.debug("Restarted LEDC");
    Haptics::globals.reinitLEDC = false;
  }

  //Haptics::LEDC::tick();
  Haptics::PCA::setPcaDuty(&Haptics::globals, &Haptics::conf);
  Haptics::SerialComm::tick();

  // Moves heavy lifting out of ISR's
  if (Haptics::globals.updatedMotors) {
    Haptics::globals.updatedMotors = false;
    Haptics::Wireless::updateMotorVals();
  }

  if (Haptics::globals.processOscCommand) { 
    // if we were sent a command over OSC
    messageRecieved = true;
    const String response = Haptics::parseInput(Haptics::globals.commandToProcess);
    OscMessage commandResponse(COMMAND_ADDRESS);
    commandResponse.pushString(response);
    Haptics::Wireless::oscClient.send(Haptics::Wireless::hostIP, Haptics::Wireless::sendPort, commandResponse);
    Haptics::globals.commandToProcess = "";
    Haptics::globals.processOscCommand = false;
    
  } else if (Haptics::globals.processSerCommand) { 
    // If we were sent a command over serial
    String response = Haptics::parseInput(Haptics::globals.commandToProcess);
    Serial.println(response);
    Haptics::globals.processSerCommand = false;
  }  

  ticks += 1;
  now = millis();
  if (now - lastWifiTick >= 7) {// Roughly 150hz
    Haptics::Wireless::Tick();
    lastWifiTick = now;
  }

  if (now - lastSerialPush >= 1000) {
    logger.debug("Loop/sec: %d", ticks);
    logger.debug("Temp: %.2f °C", temperatureRead());
    Haptics::Wireless::printMetrics();
    //Haptics::PwmUtils::printAllDuty();
    //Haptics::Wireless::printRawPacket();

    // we should recieve atleast one message over a second if we are connected/
    // if we arent connected we should broadcast each second
    if (now - Haptics::lastPacketMs > 1000) {
      Haptics::Wireless::Broadcast();
    }

    lastSerialPush = now;
    ticks = 0;
  }
}