// config.h
#ifndef CONFIG_H
#define CONFIG_H
#include "Arduino.h"
#include "software_defines.h"
#include "logging/Logger.h"

static const u_int NODE_MAP_SIZE = (MAX_MOTORS * ((NODE_LOCATION_DIGITS*3) + 3)) * 2;

namespace Haptics {

    /// user-configurable, persistent values
    struct Config {
        char wifi_ssid[32];
        char wifi_password[64];
        char mdns_name[12];
        char node_map[NODE_MAP_SIZE];
        uint8_t i2c_scl;
        uint8_t i2c_sda;
        uint32_t i2c_speed;
        uint16_t motor_map_i2c_num;
        uint16_t motor_map_i2c[MAX_I2C_MOTORS];
        uint16_t motor_map_ledc_num;
        uint16_t motor_map_ledc[MAX_LEDC_MOTORS];
        uint16_t config_version;
    }; 
    /// To add a value, all you have to do is add it to this struct, add a default, and insert it into the configFields list
    /// All set and get commands are handled automatically from there

    // sensible defaults
    const Config defaultConfig = {
    "SlimeServer", //ssid
    "95815480", //password
    "VRCHaptics", // name that will be displayed on the gui
    "", // Will be set via Serial or wifi.
    SCL, // scl default of board
    SDA, // sda default of board
    400000U, // i2c clock
    0, // i2c num
    {0}, 
    0,
    {0},
    CONFIG_VERSION
    };

    /// @brief Loads config to global instance
    void loadConfig();
    /// @brief Saves configuration in memory to disk
    void saveConfig();

    inline Config conf;

    // Supported field types.
    enum ConfigFieldType {
        CONFIG_TYPE_UINT8,
        CONFIG_TYPE_UINT16,
        CONFIG_TYPE_UINT32,
        CONFIG_TYPE_FLOAT,
        CONFIG_TYPE_STRING,
        CONFIG_TYPE_ARRAY,
    };

    // A descriptor for each configuration field.
    struct ConfigFieldDescriptor {
        const char* name;      // Field name (e.g., "wifi_ssid")
        size_t offset;         // Offset into the Config struct (using offsetof)
        ConfigFieldType type;  // Type of the field
        size_t size;           // For string fields: the size of the char array
        ConfigFieldType subType; // Only used when type==CONFIG_TYPE_ARRAY.
    };

    // For normal (non-array) fields.
    #define CONFIG_FIELD(field, type, size) { #field, offsetof(Config, field), type, size, CONFIG_TYPE_UINT8 }

    /// ONLY SUPPORTS Numerical fields. 
    #define CONFIG_FIELD_ARRAY(field, subType, count) { #field, offsetof(Config, field), CONFIG_TYPE_ARRAY, count, subType }

    // List of configurable fields. Add new entries here when extending the config.
    static const ConfigFieldDescriptor configFields[] = {
        CONFIG_FIELD(wifi_ssid,     CONFIG_TYPE_STRING, sizeof(((Config*)0)->wifi_ssid)),
        CONFIG_FIELD(wifi_password, CONFIG_TYPE_STRING, sizeof(((Config*)0)->wifi_password)),
        CONFIG_FIELD(mdns_name,     CONFIG_TYPE_STRING, sizeof(((Config*)0)->mdns_name)),
        CONFIG_FIELD(node_map,      CONFIG_TYPE_STRING, sizeof(((Config*)0)->node_map)),
        CONFIG_FIELD(i2c_scl,       CONFIG_TYPE_UINT8,  0),
        CONFIG_FIELD(i2c_sda,       CONFIG_TYPE_UINT8,  0),
        CONFIG_FIELD(i2c_speed,     CONFIG_TYPE_UINT32, 0),
        CONFIG_FIELD(motor_map_i2c_num, CONFIG_TYPE_UINT16, 0),
        CONFIG_FIELD_ARRAY(motor_map_i2c, CONFIG_TYPE_UINT16, MAX_I2C_MOTORS),
        CONFIG_FIELD(motor_map_ledc_num, CONFIG_TYPE_UINT16, 0),
        CONFIG_FIELD_ARRAY(motor_map_ledc, CONFIG_TYPE_UINT16, MAX_LEDC_MOTORS),
        CONFIG_FIELD(config_version, CONFIG_TYPE_UINT16, 0)
    };
    static const size_t configFieldsCount = sizeof(configFields) / sizeof(configFields[0]);

    // Helper to locate a descriptor by field name (case-insensitive).
    inline const ConfigFieldDescriptor* getConfigFieldDescriptor(const String& name) {
        for (size_t i = 0; i < configFieldsCount; i++) {
            if (name.equalsIgnoreCase(configFields[i].name)) {
                return &configFields[i];
            }
        }
        return nullptr;
    }

} // namespace Haptics

#endif // CONFIG_H