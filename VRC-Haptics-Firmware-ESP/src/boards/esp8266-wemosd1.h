#ifndef ESPC3_DEFINES_H
#define ESPC3_DEFINES_H

// Which pins are allowed to be used as LEDC (straight pwm from board)
#define SUPPORTS_LEDC
#define LEDC_POSSIBLE_PINS 5, 4, 0, 2, 14, 12, 13, 15
#define SUPPORTS_I2C
// Preferred SCL and SDA pins 0th is default scl, 1st is sda
#define I2C_POSSIBLE_DATA 5, 4
// Keep prefered speed at front
#define I2C_SPEEDS 4000000U, 100000U

#endif // DEFAULT_H
