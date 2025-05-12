#include "callbacks.h"


namespace Haptics  {
namespace Wireless { 

    bool printNext = false;

    void printRaw() {
        printNext = true;
    }

    /// @brief sets the individual ledc and i2c maps from the global maps
    /// i2c_num -> 4
    /// ledc_num -> 2
    ///  0, 1, 2, 3, 4, 5, (6 numbers transmitted)
    /// [0, 1]              //ledc 
    ///       [2, 3, 4, 5]  //i2c
    void updateMotorVals(){
        lastPacketMs = millis();

        const uint16_t totalMotors = Haptics::conf.motor_map_i2c_num+Haptics::conf.motor_map_ledc_num;
        for (uint16_t i = 0; i < totalMotors; i++) {
            if (Haptics::conf.motor_map_i2c_num && Haptics::conf.motor_map_ledc_num) { // both i2c and ledc motors
                // haven't got through the ledc motors
                if (i < Haptics::conf.motor_map_ledc_num) {
                    Haptics::globals.ledcMotorVals[i] = Haptics::globals.allMotorVals[i] >> 8;
                } else { // past ledc
                    Haptics::globals.pcaMotorVals[i-Haptics::conf.motor_map_ledc_num] = Haptics::globals. allMotorVals[i];
                }
            } else if (Haptics::conf.motor_map_i2c_num) {// if only i2c
                Haptics::globals.pcaMotorVals[i] = Haptics::globals.allMotorVals[i];
            } else {// assume it's only ledc
                Haptics::globals.ledcMotorVals[i-conf.motor_map_i2c_num] = Haptics::globals.allMotorVals[i] >> 8;
            }
        }
    }

    void motorMessage_callback(const OscMessage& message){
        lastPacketMs = millis();
        
        if (first_packet){
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
        for (uint16_t i = 0; i < numElements; i++) {
            memcpy(snippet, &msg_char[OSC_MOTOR_CHAR_NUM * i], OSC_MOTOR_CHAR_NUM);
            snippet[OSC_MOTOR_CHAR_NUM] = '\0'; // null terminate the snippet
            // convert a section of the input string into an integer number
            Haptics::globals.allMotorVals[i] = strtol(snippet, NULL, 16);
        }

        // push the changes to the individual motor array's outside of ISR time
        Haptics::globals.updatedMotors = true;
        
    }

    void commandMessageCallback(const OscMessage& msg) {
        // schedule processing the command on the next cycle.
        Haptics::globals.commandToProcess = msg.arg<String>(0);
        Haptics::globals.processOscCommand = true;
    }


    void printOSCMessage(const OscMessage& message) {
        Serial.print("Address: ");
        Serial.println(message.address());
        for (size_t i = 0; i < message.size(); i++) {
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
