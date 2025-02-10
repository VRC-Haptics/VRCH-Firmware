namespace Haptics { 
// esp32c3-supermini defines
namespace BoardDefines {
    // Which pins are allowed to be used as LEDC (straight pwm from board)
    #define SUPPORTS_LEDC
    #define LEDC_POSSIBLE_PINS 4, 3, 2, 1, 5, 6, 7, 8, 9, 10
    #define SUPPORTS_I2C
    // Preferred SCL and SDA pins (is a list with 0th index as default)
    #define I2C_POSSIBLE_SCL 9
    #define I2C_POSSIBLE_SDA 8
    // Keep prefered speed at front
    #define I2C_SPEEDS 4000000U, 100000U
} // namespace BoardDefines
} // namespace Haptics