#ifndef ESPC3_DEFINES_H
#define ESPC3_DEFINES_H

// Which pins are allowed to be used as LEDC (straight pwm from board)
#define SUPPORTS_LEDC
#define LEDC_POSSIBLE_PINS 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 33, 34, 37, 38,  36, 35, 48, 47, 42, 41, 40, 39
// 17, 18, 45, 
#define SUPPORTS_I2C
// Preferred SCL and SDA pins 0th is default scl, 1st is sda
#define I2C_POSSIBLE_DATA 9, 8
#define I2C_SPEEDS 4000000U, 100000U
#define LEDC_TIMER 3

#endif // DEFAULT_H
