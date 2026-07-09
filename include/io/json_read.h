#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace utils {

inline std::string read_json(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) throw std::runtime_error("Failed to open file: " + file_path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
    
} //namespace utils