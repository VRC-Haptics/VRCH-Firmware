#ifndef SOFTWARE_DEFINES_H
#define SOFTWARE_DEFINES_H

/// Almost all numbers and constants should end up here

/// Motor defines (define JSON_SIZE if more than 128 total)
#define MAX_I2C_MOTORS  64
#define MAX_LEDC_MOTORS 64

// pwm frequency of pca motors
#define PCA_FREQUENCY 1500 
#define PCA_1 0x40 
#define PCA_2 0x41

/// defaults from senseshift
#define LEDC_FREQUENCY 60
#define LEDC_RESOLUTION 8

/// Wireless defines
#define OSC_MOTOR_CHAR_NUM 4
#define RECIEVE_PORT 1027

#define HEARTBEAT_ADDRESS "/hrtbt"
#define PING_ADDRESS "/ping"
#define COMMAND_ADDRESS "/command"
#define MOTOR_ADDRESS "/h"

#define MDNS_SERVICE_NAME "haptics"
#define MDNS_SERVICE_PROTOCOL "udp"

// internal (calculated for 64 motors on each)
#define JSON_SIZE 4096

#define CONFIG_VERSION 1

#endif // Software defines