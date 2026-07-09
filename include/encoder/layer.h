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