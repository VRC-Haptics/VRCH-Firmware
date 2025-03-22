#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "config.h"  // has Config and defaultConfig
#include "config_parser.h"
#include "logging/Logger.h"

namespace Haptics {

    Logging::Logger logger("Haptics");

    void loadConfig() {
        // Check if the config file exists.
        if (!LittleFS.exists("/config.json")) {
            logger.debug("Config file not found. Creating default config.");
            memcpy(&conf, &defaultConfig, sizeof(Config));
            saveConfig();  // write the default config to file
            return;
        }

        File configFile = LittleFS.open("/config.json", "r");
        if (!configFile) {
            logger.error("Failed to open config file for reading.");
            memcpy(&conf, &defaultConfig, sizeof(Config));
            return;
        }

        DynamicJsonDocument doc(JSON_SIZE);
        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();
        if (error) {
            logger.error("Failed to parse config file, using default config.");
            memcpy(&conf, &defaultConfig, sizeof(Config));
            return;
        }

        // For each configurable field, load its value (or fallback to default)
        for (size_t i = 0; i < configFieldsCount; i++) {
            const ConfigFieldDescriptor &field = configFields[i];
            uint8_t *confFieldPtr = ((uint8_t*)&conf) + field.offset;
            // Check whether the field exists in the JSON document
            if (doc.containsKey(field.name)) {
                switch (field.type) {
                    case CONFIG_TYPE_STRING: {
                        const char* value = doc[field.name] | ((const char*)(((uint8_t*)&defaultConfig) + field.offset));
                        // Make sure not to overflow the destination
                        strncpy((char*)confFieldPtr, value, field.size);
                        break;
                    }
                    case CONFIG_TYPE_UINT8: {
                        *((uint8_t*)confFieldPtr) = doc[field.name] | *((uint8_t*)(((uint8_t*)&defaultConfig) + field.offset));
                        break;
                    }
                    case CONFIG_TYPE_UINT16: {
                        *((uint16_t*)confFieldPtr) = doc[field.name] | *((uint16_t*)(((uint8_t*)&defaultConfig) + field.offset));
                        break;
                    }
                    case CONFIG_TYPE_UINT32: {
                        *((uint32_t*)confFieldPtr) = doc[field.name] | *((uint32_t*)(((uint8_t*)&defaultConfig) + field.offset));
                        break;
                    }
                    case CONFIG_TYPE_FLOAT: {
                        *((float*)confFieldPtr) = doc[field.name] | *((float*)(((uint8_t*)&defaultConfig) + field.offset));
                        break;
                    }
                    case CONFIG_TYPE_ARRAY: {
                        JsonArray arr = doc[field.name].as<JsonArray>();
                        if (arr) {
                            // Here we assume the array is of type uint16_t (adjust if necessary)
                            for (size_t j = 0; j < field.size && j < arr.size(); j++) {
                                ((uint16_t*)confFieldPtr)[j] = arr[j] | (((uint16_t*)(((uint8_t*)&defaultConfig) + field.offset))[j]);
                            }
                        } else {
                            // Field exists in the JSON but is not an array? Fallback to default.
                            memcpy(confFieldPtr, ((uint8_t*)&defaultConfig) + field.offset, field.size * sizeof(uint16_t));
                        }
                        break;
                    }
                    default:
                        logger.error("Unsupported config field type for field:");
                        logger.error(field.name);
                        break;
                }
            } else {
                // Field not present in file: use default value
                uint8_t *defaultFieldPtr = ((uint8_t*)&defaultConfig) + field.offset;
                // For strings use strncpy; for numbers or arrays, copy the raw bytes
                if (field.type == CONFIG_TYPE_STRING) {
                    strncpy((char*)confFieldPtr, (const char*)defaultFieldPtr, field.size);
                } else if (field.type == CONFIG_TYPE_ARRAY) {
                    memcpy(confFieldPtr, defaultFieldPtr, field.size * sizeof(uint16_t));
                } else {
                    // For scalars we assume the size is that of the type
                    memcpy(confFieldPtr, defaultFieldPtr, sizeof(uint32_t));  // works for uint8_t, uint16_t, uint32_t, float
                }
            }
        }

        // Check for a config version mismatch (or missing config_version indicates an old file)
        uint16_t fileVersion = doc["config_version"] | 0;
        if (fileVersion < defaultConfig.config_version || defaultConfig.config_version == 0) {
            logger.debug("Config version outdated. Merging new defaults and updating file.");
            saveConfig();
        }

        logger.debug("Loaded config:");
        serializeJsonPretty(doc, Serial);
        Serial.println();
    }

    // Updated saveConfig() function
    void saveConfig() {
        DynamicJsonDocument doc(JSON_SIZE);

        // For each configurable field, write its value from conf to the JSON document.
        for (size_t i = 0; i < configFieldsCount; i++) {
            const ConfigFieldDescriptor &field = configFields[i];
            uint8_t *confFieldPtr = ((uint8_t*)&conf) + field.offset;
            switch (field.type) {
                case CONFIG_TYPE_STRING: {
                    doc[field.name] = (const char*)confFieldPtr;
                    break;
                }
                case CONFIG_TYPE_UINT8: {
                    doc[field.name] = *((uint8_t*)confFieldPtr);
                    break;
                }
                case CONFIG_TYPE_UINT16: {
                    doc[field.name] = *((uint16_t*)confFieldPtr);
                    break;
                }
                case CONFIG_TYPE_UINT32: {
                    doc[field.name] = *((uint32_t*)confFieldPtr);
                    break;
                }
                case CONFIG_TYPE_FLOAT: {
                    doc[field.name] = *((float*)confFieldPtr);
                    break;
                }
                case CONFIG_TYPE_ARRAY: {
                    // For array fields, we create a nested JSON array.
                    JsonArray arr = doc.createNestedArray(field.name);
                    // Assuming here the array contains uint16_t elements.
                    for (size_t j = 0; j < field.size; j++) {
                        arr.add(((uint16_t*)confFieldPtr)[j]);
                    }
                    break;
                }
                default:
                    logger.error("Unsupported config field type during save for field:");
                    logger.error(field.name);
                    break;
            }
        }

        // Write the entire config document to file.
        File configFile = LittleFS.open("/config.json", "w");
        if (configFile) {
            serializeJson(doc, configFile);
            configFile.close();
            logger.debug("Configuration saved.");
            serializeJsonPretty(doc, Serial);
            Serial.println();
        } else {
            logger.error("Failed to open config file for writing.");
        }
    }
} // namespace Haptics
