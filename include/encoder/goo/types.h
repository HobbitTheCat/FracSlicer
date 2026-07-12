#pragma once

#include <cstddef>
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

namespace encoder {
    using Millimeters = float;
    using Seconds = float;
    using MillimetersPerMinute = float;

    enum class ExposureDelayMode : uint8_t {
        TurnOffTime = 0,
        StaticTime = 1
    };

    template<typename T>
    struct Vector2 {
        T x{0};
        T y{0};
    };

    template<typename T>
    struct Vector3 {
        T x{0};
        T y{0};
        T z{0};
    };

    template <std::size_t N>
    struct SizedString {
        std::array<uint8_t, N> data {};

        SizedString(const std::string& str = "") {
            std::size_t copy_len = std::min(str.length(), N);
            std::memcpy(data.data(), str.data(), copy_len);
        }
    };

    // Pixel format - 16 bit: 0bRRRRRGGGGGGBBBBB (RGB565)
    template <std::size_t Width, std::size_t Height>
    struct PreviewImage {
        std::vector<uint16_t> data;
        PreviewImage() {
            data.resize(Width * Height, 0);
        }
    };

    using SmallPreview = PreviewImage<116, 116>;
    using BigPreview = PreviewImage<290, 290>;
    
} // namespace encoder