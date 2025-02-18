#include "config_parser.h"
#include "config.h"


namespace Haptics {

    // Helper: Given a pointer to conf and a descriptor, return a pointer to the field.
    inline void* getFieldPtr(const ConfigFieldDescriptor& desc) {
        return ((uint8_t*)&conf) + desc.offset;
    }

    String getArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field);
    bool setArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field, const String& input);

    /// @brief Cuts the input string into command, keys, and value tokens.
    /// @param input String to cut up
    /// @param command  String to fill with command
    /// @param key String to fill with key
    /// @param value String to fill with the value
    static void cutInput(const String &input, String &command, String &key, String &value) {
        int firstSpace = input.indexOf(' ');
        if (firstSpace == -1) {
        command = input;
        } else {
        command = input.substring(0, firstSpace);
        int secondSpace = input.indexOf(' ', firstSpace + 1);
        if (secondSpace == -1) {
            key = input.substring(firstSpace + 1);
        } else {
            key = input.substring(firstSpace + 1, secondSpace);
            value = input.substring(secondSpace + 1);
        }
        }
        // Normalize to uppercase.
        command.toUpperCase();
        key.toUpperCase();
    }

    /// @brief Handles all commands under the SET keyword
    /// @param key Which config key to set
    /// @param value Which value to set the keyword to
    /// @return Feedback to return
    String handleSet(const String &key, const String &value) {
        // "ALL DEFAULT" resets the config to default values.
        if (key.equalsIgnoreCase("ALL") && value.equalsIgnoreCase("DEFAULT")) {
            conf = defaultConfig;
            return "Config reset to default";
        }

        const ConfigFieldDescriptor* field = getConfigFieldDescriptor(key);
        if (!field) {
            return "Error: Unknown config key " + key;
        }

        void* ptr = getFieldPtr(*field);
        bool success = false;

        switch (field->type) {
            case CONFIG_TYPE_STRING:
                // don't forget the null terminator
                if (value.length() < field->size) {
                    strncpy((char*)ptr, value.c_str(), field->size);
                    success = true;
                }
                break;
            case CONFIG_TYPE_UINT8:
                *(uint8_t*)ptr = (uint8_t)value.toInt();
                success = true;
                break;
            case CONFIG_TYPE_UINT16:
                *(uint16_t*)ptr = (uint16_t)value.toInt();
                success = true;
                break;
            case CONFIG_TYPE_UINT32:
                *(uint32_t*)ptr = (uint32_t)value.toInt();
                success = true;
                break;
            case CONFIG_TYPE_FLOAT:
                *(float*)ptr = value.toFloat();
                success = true;
                break;
            case CONFIG_TYPE_ARRAY:
                success = setArrayFieldValue(ptr, *field, value);
                break;
            default:
                break;
        }

        if (success) {
            return String(field->name) + " set to " + value;
        } else {
            return "Error: Failed to set " + key + " (value may be too long or invalid)";
        }
    }

    /// @brief Handles all commands under the GET keyword
    /// @param key Which config key to get
    /// @param value Currently unused
    /// @return Feedback to return
    String handleGet(const String &key, const String &value) {
        // If "ALL", build a JSON string.
        if (key.equalsIgnoreCase("ALL")) {
            String json = "{";
            for (size_t i = 0; i < configFieldsCount; i++) {
                const ConfigFieldDescriptor& field = configFields[i];
                void* ptr = getFieldPtr(field);
                String fieldVal;
                bool outputQuotes = false; // controls whether to wrap fieldVal in quotes
    
                switch (field.type) {
                    case CONFIG_TYPE_STRING:
                        fieldVal = String((char*)ptr);
                        outputQuotes = true;
                        break;
                    case CONFIG_TYPE_UINT8:
                        fieldVal = String(*(uint8_t*)ptr);
                        break;
                    case CONFIG_TYPE_UINT16:
                        fieldVal = String(*(uint16_t*)ptr);
                        break;
                    case CONFIG_TYPE_UINT32:
                        fieldVal = String(*(uint32_t*)ptr);
                        break;
                    case CONFIG_TYPE_FLOAT:
                        fieldVal = String(*(float*)ptr);
                        break;
                    case CONFIG_TYPE_ARRAY:
                        fieldVal = getArrayFieldValue(ptr, field);
                        // Only wrap array elements in quotes if the array's subtype is string.
                        if (field.subType == CONFIG_TYPE_STRING) {
                            outputQuotes = true;
                        }
                        break;
                }

                json += "\"" + String(field.name) + "\":";
                if (outputQuotes) {
                    json += "\"" + fieldVal + "\"";
                } else {
                    json += fieldVal;
                }
                if (i < configFieldsCount - 1)
                    json += ",";
                }
            json += "}";
            return json;
        }

        // Otherwise, locate the descriptor for the given key.
        const ConfigFieldDescriptor* field = getConfigFieldDescriptor(key);
        if (!field) {
            return "Error: Unknown config key " + key;
        }

        String result = ""; // Throws fit if not out here
        void* ptr = getFieldPtr(*field);
        switch (field->type) {
            case CONFIG_TYPE_STRING:
                return String((char*)ptr);
            case CONFIG_TYPE_UINT8:
                return String(*(uint8_t*)ptr);
            case CONFIG_TYPE_UINT16:
                return String(*(uint16_t*)ptr);
            case CONFIG_TYPE_UINT32:
                return String(*(uint32_t*)ptr);
            case CONFIG_TYPE_FLOAT:
                return String(*(float*)ptr);
            case CONFIG_TYPE_ARRAY: {
                uint16_t* arr = (uint16_t*)((uint8_t*)&conf + field->offset);
                for (size_t i = 0; i < field->size; i++) {
                    result += String(arr[i]);
                    if (i < field->size - 1)
                        result += ",";
                }
                return result;
            }
            default:
                return "Error: Unsupported type";
        }
    }

    String getArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field) {
        String result = "[";
        // Assume field.size is the number of elements in the array.
        for (size_t j = 0; j < field.size; j++) {
            if (j > 0) {
                result += ",";
            }
            switch (field.subType) {
                case CONFIG_TYPE_UINT8: {
                    uint8_t* arr = (uint8_t*)ptr;
                    result += String(arr[j]);
                    break;
                }
                case CONFIG_TYPE_UINT16: {
                    uint16_t* arr = (uint16_t*)ptr;
                    result += String(arr[j]);
                    break;
                }
                case CONFIG_TYPE_UINT32: {
                    uint32_t* arr = (uint32_t*)ptr;
                    result += String(arr[j]);
                    break;
                }
                case CONFIG_TYPE_FLOAT: {
                    float* arr = (float*)ptr;
                    result += String(arr[j]);
                    break;
                }
                default:
                    result += "\"?\"";
                    break;
            }
        }
        result += "]";
        return result;
    }

    // Helper function to set an array field from a CSV string.
    bool setArrayFieldValue(void* ptr, const ConfigFieldDescriptor &field, const String& input) {
        size_t count = 0;
        int start = 0;
        // Process each token (assumes comma-separated values)
        while (true) {
            int commaIndex = input.indexOf(',', start);
            String token;
            if (commaIndex == -1) {
                token = input.substring(start);
            } else {
                token = input.substring(start, commaIndex);
            }
            token.trim();
            if (token.length() > 0) {
                // Only write if we haven't exceeded the capacity.
                if (count < field.size) {
                    switch (field.subType) {
                        case CONFIG_TYPE_UINT8: {
                            uint8_t* arr = (uint8_t*)ptr;
                            arr[count] = (uint8_t) token.toInt();
                            break;
                        }
                        case CONFIG_TYPE_UINT16: {
                            uint16_t* arr = (uint16_t*)ptr;
                            arr[count] = (uint16_t) token.toInt();
                            break;
                        }
                        case CONFIG_TYPE_UINT32: {
                            uint32_t* arr = (uint32_t*)ptr;
                            arr[count] = (uint32_t) token.toInt();
                            break;
                        }
                        case CONFIG_TYPE_FLOAT: {
                            float* arr = (float*)ptr;
                            arr[count] = token.toFloat();
                            break;
                        }
                        default:
                            return false;
                    }
                    count++;
                } else {
                    // More tokens provided than the array can hold.
                    return false;
                }
            }
            if (commaIndex == -1) break;
            start = commaIndex + 1;
        }
        // Optionally, clear out any remaining elements.
        for (size_t i = count; i < field.size; i++) {
            switch (field.subType) {
                case CONFIG_TYPE_UINT8: {
                    uint8_t* arr = (uint8_t*)ptr;
                    arr[i] = 0;
                    break;
                }
                case CONFIG_TYPE_UINT16: {
                    uint16_t* arr = (uint16_t*)ptr;
                    arr[i] = 0;
                    break;
                }
                case CONFIG_TYPE_UINT32: {
                    uint32_t* arr = (uint32_t*)ptr;
                    arr[i] = 0;
                    break;
                }
                case CONFIG_TYPE_FLOAT: {
                    float* arr = (float*)ptr;
                    arr[i] = 0.0f;
                    break;
                }
                default:
                    break;
            }
        }
        // Return success if at least one token was processed.
        return (count > 0);
    }

    /// @brief Parses input and manages the configuration accordingly
    /// @param input the input string to be processed for a command
    /// @return The return message
    String parseInput(const String &input) {
        // get tokens
        String command, key, value, feedback;
        cutInput(input, command, key, value);

        // parse each command
        if (command == "SET") {
            feedback = handleSet(key, value);
            saveConfig();
        } else if (command == "GET") {
            feedback = handleGet(key, value);
        } else {
            feedback = "Unknown command: "+ input;
        }

        return feedback;
    }

}