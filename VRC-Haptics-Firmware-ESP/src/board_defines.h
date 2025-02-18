#ifndef BOARD_DEFINES_H
#define BOARD_DEFINES_H

// switch between board imports
#if defined(BOARD_ESP32C3_SUPERMINI)
#include "boards/esp32c3-supermini.h"
#elif defined(BOARD_ESP32S3_SUPERMINI)
#include "boards/esp32s3-supermini.h"
#elif defined(BOARD_ESP32)
#include "boards/esp32.h"
#elif defined(BOARD_ESP8266_WEMOSD1)
#include "boards/esp8266-wemosd1.h"
#else 
#include "boards/default.h"
#warning "no specific board defined, using default values. This typically isn't desired behaviour"
#endif

#endif // BOARD_DEFINES_H
