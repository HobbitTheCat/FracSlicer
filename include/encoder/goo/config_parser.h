#pragma once

#include "header.h"
#include "layer.h"

#include <string>

namespace encoder {
namespace config_parser {

    /**
     * @brief Loads printer data from JSON
     * 
     * TODO add layer configuration loading support 
     */
    Header parse_header(const std::string& file_path);

    void parse_layer(const std::string& file_path, Layer& bottom, Layer& normal);
    
} //namespace config_parser

} //namespace encoder