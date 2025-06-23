#include <Arduino.h>
#include "Wire.h"
#include "LittleFS.h"
#include <esp_wifi.h>
#include <esp_bt.h>

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

void setup()
{
	Serial.begin(115200);

#ifdef DEV_MODE
	// wait for serial if we are developing
	delay(700);
#endif

// Initialize LittleFS
#if defined(ESP8266)
	if (!LittleFS.begin())
	{ // ESP8 doesnt? have the format fucntion i guess
		logger.error("LittleFS mount failed, please restart");
		return;
	}
#else
	if (!LittleFS.begin(true))
	{
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

void coldBoot(uint64_t us_delay = 2000) // 2 ms timer by default
{
	/* power‑down every RTC power domain */
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);

	esp_sleep_enable_timer_wakeup(us_delay);
	esp_deep_sleep_start();
}

void enterLimp()
{
	logger.warn("Thermal limit reached, entering limp mode");

	// kill radios
	esp_wifi_stop();			 // Wi‑Fi off
	esp_bt_controller_disable(); // BT off

	// throttle
	setCpuFrequencyMhz(20);

	// enter sleep mode
	esp_light_sleep_start();

	// wait until cool
	while (true)
	{
		float temp = temperatureRead();

		// if temp isn't going down put into deep-sleep
		if (temp > MAX_TEMP + 5)
		{
			esp_deep_sleep(80000000000); // timeout in one day.
			esp_deep_sleep_start();
		}
		else if (temp < MIN_TEMP_COOLDOWN)
		{
			coldBoot(); // restarts without keeping anything in memory.
		}
	}
}

uint32_t ticks = 0;
time_t now = 0;
time_t lastSerialPush = millis();
time_t lastWifiTick = millis();

// Profiler setup
#define TIMER_START uint32_t dwStart = ESP.getCycleCount();
#define TIMER_END Haptics::profiler.digitalWriteCycles += (ESP.getCycleCount() - dwStart);
uint32_t loopStart = 0;
uint32_t loopTotal = 0;
bool messageRecieved = false;

void loop()
{

	if (Haptics::globals.reinitLEDC)
	{ // prevents not defined error
		Haptics::LEDC::start(&Haptics::conf);
		logger.debug("Restarted LEDC");
		Haptics::globals.reinitLEDC = false;
	}

	Haptics::PCA::setPcaDuty(&Haptics::globals, &Haptics::conf);
	Haptics::SerialComm::tick();

	// Moves heavy lifting out of ISR's
	if (Haptics::globals.updatedMotors)
	{
		Haptics::globals.updatedMotors = false;
		Haptics::Wireless::updateMotorVals();
	}

	// Handle commands (like changing the config, not setting motor values.)
	if (Haptics::globals.processOscCommand)
	{
		// if we were sent a command over OSC
		messageRecieved = true;
		const String response = Haptics::parseInput(Haptics::globals.commandToProcess);
		OscMessage commandResponse(COMMAND_ADDRESS);
		commandResponse.pushString(response);
		Haptics::Wireless::oscClient.send(Haptics::Wireless::hostIP, Haptics::Wireless::sendPort, commandResponse);
		Haptics::globals.commandToProcess = "";
		Haptics::globals.processOscCommand = false;
	}
	else if (Haptics::globals.processSerCommand)
	{
		// If we were sent a command over serial
		String response = Haptics::parseInput(Haptics::globals.commandToProcess);
		Serial.println(response);
		Haptics::globals.processSerCommand = false;
	}

	ticks += 1;
	now = millis();
	if (now - lastWifiTick >= 7)
	{ // Roughly 150hz
		Haptics::Wireless::Tick();
		lastWifiTick = now;
	}

	if (now - lastSerialPush >= 1000)
	{
		logger.debug("Loop/sec: %d", ticks);
		Haptics::Wireless::printMetrics();
		// Haptics::PwmUtils::printAllDuty();

		float temp = temperatureRead();
		logger.debug("Temp: %.2f °C", temp);
		if (temp >= MAX_TEMP)
		{
			enterLimp();
		}

		// we should recieve atleast one message over a second if we are connected/
		// if we arent connected we should broadcast each second
		if (now - Haptics::lastPacketMs > 1000)
		{
			Haptics::Wireless::Broadcast();
		}

		lastSerialPush = now;
		ticks = 0;
	}
}