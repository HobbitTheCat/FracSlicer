#pragma once

#include "types.h"
#include <cstdint>

namespace encoder {

struct Header {
    SizedString<4> version{"V3.0"};
    // Magic tag missing
    SizedString<32> software_info{"FracSlicer"};
    SizedString<24> software_version{"0.1.a"};
    SizedString<24> file_time{""};
    SizedString<32> printer_name{"standart"};
    SizedString<32> printer_type{"Default"};
    SizedString<32> resin_profile_name{"Default Profile"};

    uint16_t anti_aliasing_level{0};
    uint16_t grey_level{0};
    uint16_t blur_level{0};

    SmallPreview small_preview;
    // Delimeter missing (String)
    BigPreview big_preview;
    // Delimeter missing (String)

    uint32_t total_layers{0};
    uint16_t x_resolution{0};
    uint16_t y_resolution{0};
    bool x_mirror{false};
    bool y_mirror{false};

    Millimeters x_size{0.0f};
    Millimeters y_size{0.0f};
    Millimeters z_size{0.0f};
    Millimeters layer_thickness{0.05f};

    Seconds exposure_time{2.5f};
    ExposureDelayMode exposure_delay_mode{ExposureDelayMode::StaticTime};
    Seconds turn_off_time{0.0f};
    Seconds bottom_before_lift_time{0.0f};
    Seconds bottom_after_lift_time{0.0f};
    Seconds bottom_after_retract_time{0.5f};
    Seconds before_lift_time{0.0f};
    Seconds after_lift_time{0.0f};
    Seconds after_retract_time{0.5f};

    Seconds bottom_exposure_time{30.0f};
    uint32_t bottom_layers{5};

    Millimeters bottom_lift_distance{5.0f};
    MillimetersPerMinute bottom_lift_speed{60.0f};
    Millimeters lift_distance{5.0f};
    MillimetersPerMinute lift_speed{60.0f};

    Millimeters bottom_retract_distance{5.0f};
    MillimetersPerMinute bottom_retract_speed{150.0f};
    Millimeters retract_distance{5.0f};
    MillimetersPerMinute retract_speed{150.0f};

    Millimeters bottom_second_lift_distance{0.0f};
    MillimetersPerMinute bottom_second_lift_speed{0.0f};
    Millimeters second_lift_distance{0.0f};
    MillimetersPerMinute second_lift_speed{0.0f};

    Millimeters bottom_second_retract_distance{0.0f};
    MillimetersPerMinute bottom_second_retract_speed{0.0f};
    Millimeters second_retract_distance{0.0f};
    MillimetersPerMinute second_retract_speed{0.0f};

    uint8_t bottom_light_pwm{255};
    uint8_t light_pwm{255};

    bool advance_mode{false}; // per_layer_settings
    uint32_t printing_time{0};
    float total_volume{0.0f};
    float total_weight{0.0f};
    float total_price{0.0f};
    SizedString<8> price_unit{"$"};

    bool grey_scale_level{true};
    uint16_t transition_layers{0};
};
    
} // namespace encoder