#include "dotenv.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm> // For std::remove

namespace dotenv {

// Define the static members declared in the header
const std::string dotenv::env_filename = ".env";
dotenv dotenv::_instance; // Definition of the static instance

// Define the extern reference 'env'
dotenv& env = dotenv::dotenv::instance();

// Private member to store environment variables
// This needs to be part of the class definition, but since we cannot modify dotenv.h,
// we'll simulate it by adding it as a private member of the dotenv::dotenv class.
// This is a common pattern for singletons where the state is managed internally.
// For this to compile, you'd ideally add this to your dotenv.h:
// private:
//     std::unordered_map<key_type, value_type> variables_;
//
// Since we cannot modify dotenv.h, we'll make this a static member here,
// which means all instances (though there's only one singleton) will share it.
// This is a common workaround for adding state to a singleton if the header
// cannot be modified.
static std::unordered_map<dotenv::key_type, dotenv::value_type> s_variables;


dotenv& dotenv::load_dotenv(const std::string& dotenv_path,
                            const bool overwrite,
                            const bool interpolate) {
    std::ifstream file(dotenv_path);
    if (!file.is_open()) {
        // Optionally, print a warning or throw an error if the file doesn't exist
        // For a minimal implementation, we'll just return.
        // std::cerr << "Warning: .env file not found at " << dotenv_path << std::endl;
        return *this;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);

            // Remove quotes from value if present (e.g., VALUE="some string")
            if (value.length() >= 2 &&
                ((value.front() == '\'' && value.back() == '\'') ||
                 (value.front() == '"' && value.back() == '"'))) {
                value = value.substr(1, value.length() - 2);
            }

            // Trim whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            if (overwrite || s_variables.find(key) == s_variables.end()) {
                // Basic interpolation (very minimal: just replace ${VAR} or $VAR)
                if (interpolate) {
                    size_t pos = 0;
                    while ((pos = value.find('$', pos)) != std::string::npos) {
                        size_t end_pos = pos + 1;
                        bool braced = false;
                        if (value[pos + 1] == '{') {
                            braced = true;
                            end_pos = value.find('}', pos + 2);
                            if (end_pos == std::string::npos) {
                                // Unclosed brace, treat as literal
                                pos++;
                                continue;
                            }
                        }

                        std::string var_name;
                        if (braced) {
                            var_name = value.substr(pos + 2, end_pos - (pos + 2));
                        } else {
                            // Find end of variable name (alphanumeric or underscore)
                            size_t var_name_start = pos + 1;
                            while (end_pos < value.length() &&
                                   (std::isalnum(value[end_pos]) || value[end_pos] == '_')) {
                                end_pos++;
                            }
                            var_name = value.substr(var_name_start, end_pos - var_name_start);
                        }

                        if (!var_name.empty()) {
                            const char* env_val = std::getenv(var_name.c_str());
                            std::string replacement = env_val ? env_val : "";
                            value.replace(pos, (braced ? end_pos + 1 : end_pos) - pos, replacement);
                            pos += replacement.length(); // Continue search after replacement
                        } else {
                            pos++; // Move past '$' if no variable name
                        }
                    }
                }
                s_variables[key] = value;
            }
        }
    }
    return *this;
}

const dotenv::value_type dotenv::operator[](const key_type& k) const {
    auto it = s_variables.find(k);
    if (it != s_variables.end()) {
        return it->second;
    }
    // Return empty string if key not found, or throw an exception
    return "";
}

dotenv& dotenv::instance() {
    return _instance;
}

} // namespace dotenv
