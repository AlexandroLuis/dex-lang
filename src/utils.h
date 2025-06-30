#ifndef DEX_UTILS_H
#define DEX_UTILS_H

#include <string>

namespace dex {

// Simple utility: trim whitespace from both ends
inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

} // namespace dex

#endif // DEX_UTILS_H
