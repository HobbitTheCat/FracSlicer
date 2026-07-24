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

#include "types.h"
#include <cstdint>
#include <vector>

namespace encoder {

struct Layer {
    bool pause{false};
    Millimeters pause_position_z{0.0f};
    Millimeters layer_position_z{0.0f};
    Seconds layer_exposure_time{0.0f};
    Seconds layer_off_time{0.0f};
    Seconds before_lift_time{0.0f};
    Seconds after_lift_time{0.0f};
    Seconds after_retract_time{0.0f};
    Millimeters lift_distance{0.0f};
    MillimetersPerMinute lift_speed{0.0f};
    Millimeters second_lift_distance{0.0f};
    MillimetersPerMinute second_lift_speed{0.0f};
    Millimeters retract_distance{0.0f};
    MillimetersPerMinute retract_speed{0.0f};
    Millimeters second_retract_distance{0.0f};
    MillimetersPerMinute second_retract_speed{0.0f};
    uint8_t light_pwm{255};
    // Delimeter missing (String)

    std::vector<uint8_t> data;
    uint8_t checksum{0};
};
    
} // namespace encoder