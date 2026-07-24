/*
 * Copyright (C) 2026 Basilisk
 *
 * Ported to C++ from msla_format v0.2.0
 * Original repository: https://github.com/connorslade/mslicer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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