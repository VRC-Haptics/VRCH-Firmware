#ifndef SOFTWARE_DEFINES_H
#define SOFTWARE_DEFINES_H

/// Almost all numbers and constants should end up here

/// Motor defines (define JSON_SIZE if more than 128 total)
#define MAX_I2C_MOTORS  64
#define MAX_LEDC_MOTORS 64
#define MAX_MOTORS MAX_I2C_MOTORS + MAX_LEDC_MOTORS

// pwm frequency of pca motors
#define PCA_FREQUENCY 1500 
#define PCA_1 0x40 
#define PCA_2 0x41

/// parameters to drive direct pins at
#define LEDC_FREQUENCY 300
#define LEDC_RESOLUTION 8 // Don't just change this. Reimplemnt the array and scaling too.

/// Temperature controls
#define MAX_TEMP 120.0
#define MIN_TEMP_COOLDOWN 80.0

/// Wireless defines
#define OSC_MOTOR_CHAR_NUM 4
#define RECIEVE_PORT 1027
#define MULTICAST_PORT 8888
#define MULTICAST_GROUP 239,0,0,1

#define HEARTBEAT_ADDRESS "/hrtbt"
#define PING_ADDRESS "/ping"
#define COMMAND_ADDRESS "/command"
#define MOTOR_ADDRESS "/h"

// internal (calculated for 64 motors on each)
#define JSON_SIZE 4096
#define NODE_LOCATION_DIGITS 4 
#define MAX_NODE_GROUPS 10

#define CONFIG_VERSION 1

#endif // Software defines