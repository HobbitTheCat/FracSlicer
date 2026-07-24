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

#include "encoder/layer_encoder.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace encoder {

    void LayerEncoder::add_run(uint32_t length, uint8_t value) {
        if (length == 0) return;

        int16_t diff = static_cast<int16_t>(value) - static_cast<int16_t>(last_value);
        uint8_t chunk_type;

        if (value == 0x00) {
            chunk_type = 0b00;
        } else if (value == 0xFF) {
            chunk_type = 0b11;
        } else if (!data.empty() && std::abs(diff) <= 15) {
            if (length > 255) {
                add_run(255, value);
                add_run(length - 255, value);
                return;
            }

            uint8_t is_neg = (diff < 0) ? 1 : 0;
            uint8_t len_not_1 = (length != 1) ? 1 : 0;
            uint8_t abs_diff = static_cast<uint8_t>(std::abs(diff));

            // 0babcccc
            // a => 0: add diff, 1: sub diff
            // b => 0: length of 1, 1: length is next byte
            // c => the diff
            uint8_t byte0 = (0b10 << 6) | (is_neg << 5) | (len_not_1 << 4) | abs_diff;
            this->data.push_back(byte0);
    
            if (length != 1) {
                this->data.push_back(static_cast<uint8_t>(length));
            }
    
            this->last_value = value;
            return;
        } else {
            chunk_type = 0b01;
        }

        // We determine the size of the Byte0[5:4] field
        uint8_t chunk_length_size;
        if (length <= 0x0000000F) {
            chunk_length_size = 0b00;
        } else if (length <= 0x00000FFF) {
            chunk_length_size = 0b01;
        } else if (length <= 0x000FFFFF) {
            chunk_length_size = 0b10;
        } else if (length <= 0x0FFFFFFF) {
            chunk_length_size = 0b11;
        } else {
            add_run(0x0FFFFFFF, value);
            add_run(length - 0x0FFFFFFF, value);
            return;
        }

        uint8_t byte0 = (chunk_type << 6) | (chunk_length_size << 4) | (length & 0x0F);
        this->data.push_back(byte0);
        
        // Record the color if it is not pure 0x00 or 0xFF
        if (chunk_type == 0b01) {
            this->data.push_back(value);
        }

        if (chunk_length_size == 1) {
            this->data.push_back(static_cast<uint8_t>(length >> 4));
        } else if (chunk_length_size == 2) {
            this->data.push_back(static_cast<uint8_t>(length >> 12));
            this->data.push_back(static_cast<uint8_t>(length >> 4));
        } else if (chunk_length_size == 3) {
            this->data.push_back(static_cast<uint8_t>(length >> 20));
            this->data.push_back(static_cast<uint8_t>(length >> 12));
            this->data.push_back(static_cast<uint8_t>(length >> 4));
        }
    
        this->last_value = value;    
    }
    
} //namespace encoder