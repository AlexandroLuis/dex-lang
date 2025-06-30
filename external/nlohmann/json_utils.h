#pragma once
#include <string>
#include "json.hpp"  // nlohmann json header

namespace dex {

using json = nlohmann::json;

// Convert JSON string to nlohmann::json object
json parseJSON(const std::string& jsonStr);

// Convert JSON object to string
std::string toJSON(const json& j);

}
