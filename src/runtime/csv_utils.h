#pragma once
#include <string>
#include <vector>

namespace dex {

// Parses CSV string to 2D vector of strings (rows/cols)
std::vector<std::vector<std::string>> parseCSV(const std::string& csvStr);

// Converts 2D vector of strings to CSV string
std::string toCSV(const std::vector<std::vector<std::string>>& data);

}
