#pragma once
#include <string>

namespace dex {
    std::string readFile(const std::string& filename);
    void writeFile(const std::string& filename, const std::string& data);
}
