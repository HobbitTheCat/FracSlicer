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

#include "constants.h"
#include <vector>
#include <cstdint>

namespace encoder {

class LayerEncoder{
private:
    std::vector<uint8_t> data;
    uint8_t last_value{0};
public:
    LayerEncoder() = default;

    void add_run(uint32_t length, uint8_t value);
    
    uint8_t get_checksum() const {
        return calculate_checksum(this->data.data(), this->data.size());
    }

    const std::vector<uint8_t>& get_data() const {return this->data;}

    std::vector<uint8_t> take_data() {return std::move(this->data);}
};
    
} //namespace encoder