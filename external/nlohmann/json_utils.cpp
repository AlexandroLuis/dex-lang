#include "json_utils.h"

namespace dex {

json parseJSON(const std::string& jsonStr) {
    return json::parse(jsonStr);
}

std::string toJSON(const json& j) {
    return j.dump(4); // pretty print with 4 spaces indent
}

}
