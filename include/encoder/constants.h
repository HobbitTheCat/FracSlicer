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

#include <cstdint>
#include <array>
#include <cstddef>

namespace encoder {
namespace constants {
        
    constexpr std::array<uint8_t, 11> ENDING_STRING = {
        0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x44, 0x4C, 0x50, 0x00
    };

    constexpr std::array<uint8_t, 8> MAGIC_TAG = {
        0x07, 0x00, 0x00, 0x00, 0x44, 0x4C, 0x50, 0x00
    };

    constexpr std::array<uint8_t, 2> DELIMITER = {
        0x0D, 0x0A
    };

    // V3.0 Header fixed size
    constexpr std::size_t HEADER_SIZE = 0x2FB95;

} // namespace constants

    inline uint8_t calculate_checksum(const uint8_t* data, std::size_t size) {
        uint8_t out = 0;
        for (std::size_t i = 0; i < size; i++) {
            out += data[i];
        }
        return ~out;
    }

} // namespace encoder