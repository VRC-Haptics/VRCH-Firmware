#ifndef ESPC3_DEFINES_H
#define ESPC3_DEFINES_H

// Which pins are allowed to be used as LEDC (straight pwm from board)
#define SUPPORTS_LEDC
#define LEDC_POSSIBLE_PINS 32, 33, 25, 26

#define SUPPORTS_I2C
// Preferred SCL and SDA pins 0th is default scl, 1st is sda
#define I2C_POSSIBLE_DATA 9, 8
#define I2C_SPEEDS 4000000U, 100000U
#define LEDC_TIMER 3

#endif // DEFAULT_H
