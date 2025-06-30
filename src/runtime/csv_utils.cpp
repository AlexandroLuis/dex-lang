#include "csv_utils.h"
#include <sstream>

namespace dex {

// Note: simple CSV parsing, no quoted fields support for brevity
std::vector<std::vector<std::string>> parseCSV(const std::string& csvStr) {
    std::vector<std::vector<std::string>> rows;
    std::istringstream ss(csvStr);
    std::string line;

    while (std::getline(ss, line)) {
        std::vector<std::string> cols;
        std::istringstream line_ss(line);
        std::string cell;

        while (std::getline(line_ss, cell, ',')) {
            cols.push_back(cell);
        }
        rows.push_back(cols);
    }
    return rows;
}

std::string toCSV(const std::vector<std::vector<std::string>>& data) {
    std::ostringstream ss;
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            ss << row[i];
            if (i != row.size() -1) ss << ",";
        }
        ss << "\n";
    }
    return ss.str();
}

}
