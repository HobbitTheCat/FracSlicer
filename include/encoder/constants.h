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