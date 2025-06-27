#include "callbacks.h"

namespace Haptics
{
    namespace Wireless
    {

        bool printNext = false;

        void printRaw()
        {
            printNext = true;
        }

        inline void setLedcMotor(uint16_t *index, uint16_t val)
        {
            Haptics::globals.ledcMotorVals[*index] = val >> 8;
        }

        inline void setI2CMotor(uint16_t *index, uint16_t val)
        {
            Haptics::globals.ledcMotorVals[*index] = val;
        }

        /// @brief  Handles when value is below threshold. Relys on externally resetting hasBumped when value touches zero.
        /// @param index the index of the **LEDC MOTORS** we are addressing, NOT `allMotorVals` index
        /// @param val value that was commanded.
        /// @param thresh Triggers bump when threshold below value.
        /// @param hasBumped Whether this index has been bumped since it last reached zero.
        void handleLedcBump(uint16_t *index, uint16_t *val, uint16_t *thresh, bool *hasBumped, const int64_t now, int64_t *timeBumpStart)
        {
            if (hasBumped)
                return;

            // if bumpTime is zero, we need to initiate one
            if (Haptics::Conf::conf.bump_time_us) {
                logger.debug("Starting bump to: %u", thresh);
                *timeBumpStart = now;
                setLedcMotor(index, UINT16_MAX);

                // bumpTime has not elapsed yet
            } else if (now - *timeBumpStart < Haptics::Conf::conf.bump_time_us) {
                setLedcMotor(index, UINT16_MAX);

                // bumpTime is elapsed
            } else {
                logger.debug("Finished bump to: %u", val);
                *hasBumped = true;
                Haptics::Conf::conf.bump_start_threshold = 0;
                setLedcMotor(index, *val);
            }
        }

        /// @brief  Handles when value is below threshold. Relys on externally resetting hasBumped when value touches zero.
        /// @param index the index of the **I2C MOTORS** we are addressing, NOT `allMotorVals` index
        /// @param val value that was commanded.
        /// @param thresh Triggers bump when threshold below value.
        /// @param hasBumped Whether this index has been bumped since it last reached zero.
        void handleI2CBump(uint16_t *index, uint16_t *val, uint16_t *thresh, bool *hasBumped, const int64_t now, int64_t *timeBumpStart)
        {
            if (hasBumped)
                return;

            // if bumpTime is zero, we need to initiate one
            if (Haptics::Conf::conf.bump_time_us) {
                logger.debug("Starting bump to: %u", thresh);
                *timeBumpStart = now;
                setI2CMotor(index, UINT16_MAX);

                // bumpTime has not elapsed yet
            } else if (now - *timeBumpStart < Haptics::Conf::conf.bump_time_us) {
                setI2CMotor(index, UINT16_MAX);

                // bumpTime is elapsed
            } else {
                logger.debug("Finished bump to: %u", val);
                *hasBumped = true;
                Haptics::Conf::conf.bump_start_threshold = 0;
                setI2CMotor(index, *val);
            }
        }

        void handleLEDCIndex(uint16_t i, const int64_t now, uint16_t global_i) {
            int64_t *startBumpTime = &Haptics::globals.bumpActivateTime[global_i];
            uint16_t* val = &Haptics::globals.allMotorVals[global_i];
            uint16_t* thresh = &Haptics::Conf::conf.bump_start_threshold;
            bool *hasBumped = &Haptics::globals.bumpSinceZero[global_i];

            if (val == 0)
            {
                *hasBumped = false;
                setLedcMotor(&i, *val);
            }
            else if (*val > *thresh)
            {
                *hasBumped = true;
                setLedcMotor(&i, *val);
            }
            else if (hasBumped)
            {
                setLedcMotor(&i, *val);
            } 
            else
            {
                handleLedcBump(&i, val, thresh, hasBumped, now, startBumpTime);
            }
        }

        /// @brief Handles anI2c motor index with a given global map index.
        /// @param i the I2C's motor index.
        /// @param now the current timestamp to evaluate at
        /// @param val the motor pwm strength
        void handleI2CIndex(uint16_t i, const int64_t now, uint16_t global_i) {
            int64_t *startBumpTime = &Haptics::globals.bumpActivateTime[global_i];
            uint16_t* val = &Haptics::globals.allMotorVals[global_i];
            uint16_t* thresh = &Haptics::Conf::conf.bump_start_threshold;
            bool *hasBumped = &Haptics::globals.bumpSinceZero[global_i];

            if (val == 0)
            {
                *hasBumped = false;
                setI2CMotor(&i, *val);
            }
            else if (*val > *thresh)
            {
                *hasBumped = true;
                setI2CMotor(&i, *val);
            }
            else if (hasBumped)
            {
                setI2CMotor(&i, *val);
            } 
            else
            {
                handleI2CBump(&i, val, thresh, hasBumped, now, startBumpTime);
            }
        }

        /// @brief sets the individual ledc and i2c maps from the global maps
        ///
        /// i2c_num -> 4
        ///
        /// ledc_num -> 2
        ///
        /// -0, 1, 2, 3, 4, 5, (6 numbers transmitted)
        ///
        /// [0, 1]--------------ledc
        ///
        /// ------[2, 3, 4, 5]--i2c
        ///
        void updateMotorVals()
        {
            lastPacketMs = millis();

            const uint16_t totalMotors = Haptics::Conf::conf.motor_map_i2c_num + Haptics::Conf::conf.motor_map_ledc_num;
            const int64_t bumpTime = Haptics::Conf::conf.bump_time_us;
            const int64_t now = esp_timer_get_time();
            for (uint16_t i = 0; i < totalMotors; i++)
            {
                if (Haptics::Conf::conf.motor_map_i2c_num && Haptics::Conf::conf.motor_map_ledc_num)
                { // both i2c and ledc motors
                    // haven't got through the ledc motors
                    if (i < Haptics::Conf::conf.motor_map_ledc_num)
                    {
                        handleLEDCIndex(i, now, i);
                    }
                    else
                    { // past ledc, subtract ledc to get I2C index
                        handleI2CIndex(i - Conf::conf.motor_map_ledc_num, now, i);
                    }
                }
                else if (Haptics::Conf::conf.motor_map_i2c_num)
                { // only i2c, global index equals local index
                    logger.debug("in i2c only. using global index %u for local index.", i);
                    handleI2CIndex(i, now, i);
                }
                else
                { // only ledc, global index equals local index
                    handleLEDCIndex(i, now, i);
                }
            }
        }

        void motorMessage_callback(const OscMessage &message)
        {
            lastPacketMs = millis();

            if (first_packet)
            {
                logger.debug("FIRST PACKET");
                first_packet = false;
            }

            // create char array
            String msg_str = message.arg<String>(0);
            int msg_length = msg_str.length();
            char msg_char[msg_length + 1]; // +1 for null terminator
            msg_str.toCharArray(msg_char, msg_length + 1);

            const uint8_t numElements = msg_length / OSC_MOTOR_CHAR_NUM;

            // process each hex number
            char snippet[OSC_MOTOR_CHAR_NUM + 1]; // +1 for null terminator
            for (uint16_t i = 0; i < numElements; i++)
            {
                memcpy(snippet, &msg_char[OSC_MOTOR_CHAR_NUM * i], OSC_MOTOR_CHAR_NUM);
                snippet[OSC_MOTOR_CHAR_NUM] = '\0'; // null terminate the snippet
                // convert a section of the input string into an integer number
                Haptics::globals.allMotorVals[i] = strtol(snippet, NULL, 16);
            }

            // push the changes to the individual motor array's outside of ISR time
            Haptics::globals.updatedMotors = true;
        }

        void commandMessageCallback(const OscMessage &msg)
        {
            // schedule processing the command on the next cycle.
            Haptics::globals.commandToProcess = msg.arg<String>(0);
            Haptics::globals.processOscCommand = true;
        }

        void printOSCMessage(const OscMessage &message)
        {
            Serial.print("Address: ");
            Serial.println(message.address());
            for (size_t i = 0; i < message.size(); i++)
            {
                Serial.print("Index: ");
                Serial.print((int)i);
                Serial.print("Content: ");
                Serial.print(message.arg<String>(i));
                Serial.println("");
            }
            Serial.print("\n");
        }

    } // namespace Wireless
} // namespace Haptics
