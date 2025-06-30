#include "fileio.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace dex {

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Cannot open file: " + filename);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Cannot open file for writing: " + filename);
    file << data;
}

}
