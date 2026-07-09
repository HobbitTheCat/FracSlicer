#include "encoder/config_parser.h"
#include "io/json_read.h"

#include "nlohmann_json/json.hpp"

namespace encoder {
namespace config_parser {

    Header parse_header(const std::string &file_path) {
        std::string json_str = utils::read_json(file_path);
        nlohmann::json j = nlohmann::json::parse(json_str);
        
        Header header;

        if (j.contains("printer")) {
            const auto& p = j["printer"];
                    
            if (p.contains("name")) header.printer_name = p["name"].get<std::string>();
            if (p.contains("type")) header.printer_type = p["type"].get<std::string>();
            
            header.x_resolution = p.value("resolution_x", 11520);
            header.y_resolution = p.value("resolution_y", 5102);
            header.x_mirror = p.value("x_mirror", false);
            header.y_mirror = p.value("y_mirror", false);
            
            header.x_size = p.value("size_x_mm", 218.88f);
            header.y_size = p.value("size_y_mm", 122.88f);
            header.z_size = p.value("size_z_mm", 260.0f);
        }

        if (j.contains("print_profile")) {
            const auto& prof = j["print_profile"];
                    
            if (prof.contains("name")) header.resin_profile_name = prof["name"].get<std::string>();
                    
            header.layer_thickness = prof.value("layer_thickness_mm", 0.05f);
            header.bottom_layers = prof.value("bottom_layer_count", 5);
            header.transition_layers = prof.value("transition_layers", 0);
                    
            header.exposure_time = prof.value("normal_exposure_s", 2.5f);
            header.bottom_exposure_time = prof.value("bottom_exposure_s", 30.0f);
                    
            // Delays and Pauses
            // 0: Turn Off time, 1: Static time
            int delay_mode = prof.value("exposure_delay_mode", 1); 
            header.exposure_delay_mode = (delay_mode == 1) ? ExposureDelayMode::StaticTime : ExposureDelayMode::TurnOffTime;
                    
            header.turn_off_time = prof.value("turn_off_time_s", 0.0f);
            header.bottom_before_lift_time = prof.value("bottom_before_lift_time_s", 0.0f);
            header.bottom_after_lift_time = prof.value("bottom_after_lift_time_s", 0.0f);
            header.bottom_after_retract_time = prof.value("bottom_rest_time_after_retract_s", 0.5f);
                    
            header.before_lift_time = prof.value("before_lift_time_s", 0.0f);
            header.after_lift_time = prof.value("after_lift_time_s", 0.0f);
            header.after_retract_time = prof.value("rest_time_after_retract_s", 0.5f);
            
            // Distances and Speeds for the Bottom Layers
            header.bottom_lift_distance = prof.value("bottom_lift_distance_mm", 5.0f);
            header.bottom_lift_speed = prof.value("bottom_lift_speed_mm_min", 60.0f);
            header.bottom_retract_distance = prof.value("bottom_retract_distance_mm", 5.0f);
            header.bottom_retract_speed = prof.value("bottom_retract_speed_mm_min", 150.0f);
            
            header.bottom_second_lift_distance = prof.value("bottom_second_lift_distance_mm", 0.0f);
            header.bottom_second_lift_speed = prof.value("bottom_second_lift_speed_mm_min", 0.0f);
            header.bottom_second_retract_distance = prof.value("bottom_second_retract_distance_mm", 0.0f);
            header.bottom_second_retract_speed = prof.value("bottom_second_retract_speed_mm_min", 0.0f);

            // Distances and Speeds for Standard Layers
            header.lift_distance = prof.value("lift_distance_mm", 5.0f);
            header.lift_speed = prof.value("lift_speed_mm_min", 60.0f);
            header.retract_distance = prof.value("retract_distance_mm", 5.0f);
            header.retract_speed = prof.value("retract_speed_mm_min", 150.0f);
            
            header.second_lift_distance = prof.value("second_lift_distance_mm", 0.0f);
            header.second_lift_speed = prof.value("second_lift_speed_mm_min", 0.0f);
            header.second_retract_distance = prof.value("second_retract_distance_mm", 0.0f);
            header.second_retract_speed = prof.value("second_retract_speed_mm_min", 0.0f);
            
            // Light Intensity and Other Settings
            header.bottom_light_pwm = prof.value("bottom_light_pwm", 255);
            header.light_pwm = prof.value("light_pwm", 255);
            header.advance_mode = prof.value("advance_mode", false);
            header.grey_scale_level = prof.value("grey_scale_level", true);
            
            // Additional Settings
            header.anti_aliasing_level = prof.value("anti_aliasing_level", 0);
            header.grey_level = prof.value("grey_level", 0);
            header.blur_level = prof.value("blur_level", 0);
        }
        
        printf("Successfuly parsed header\n");
        return header;
    }

    void parse_layer(const std::string& file_path, Layer& bottom, Layer& normal) {
        std::string json_str = utils::read_json(file_path);
        nlohmann::json j = nlohmann::json::parse(json_str);

        bottom = Layer();
        normal = Layer();

        if (j.contains("print_profile")) {
            auto& prof = j["print_profile"];
            
            normal.layer_exposure_time = prof.value("normal_exposure_s", 2.5f);
            normal.after_retract_time = prof.value("rest_time_after_retract_s", 0.5f);
            normal.lift_distance = prof.value("lift_distance_mm", 5.0f);
            normal.lift_speed = prof.value("lift_speed_mm_min", 60.0f);
            normal.retract_distance = prof.value("retract_distance_mm", 5.0f);
            normal.retract_speed = prof.value("retract_speed_mm_min", 150.0f);
            normal.light_pwm = prof.value("light_pwm", 255);

            bottom.layer_exposure_time = prof.value("bottom_exposure_s", 30.0f);
            bottom.after_retract_time = prof.value("bottom_rest_time_after_retract_s", 0.5f);
            bottom.lift_distance = prof.value("bottom_lift_distance_mm", 5.0f);
            bottom.lift_speed = prof.value("bottom_lift_speed_mm_min", 60.0f);
            bottom.retract_distance = prof.value("bottom_retract_distance_mm", 5.0f);
            bottom.retract_speed = prof.value("bottom_retract_speed_mm_min", 150.0f);
            bottom.light_pwm = prof.value("bottom_light_pwm", 255);
            
            normal.before_lift_time = prof.value("before_lift_time_s", 0.0f);
            normal.after_lift_time = prof.value("after_lift_time_s", 0.0f);
            bottom.before_lift_time = prof.value("bottom_before_lift_time_s", 0.0f);
            bottom.after_lift_time = prof.value("bottom_after_lift_time_s", 0.0f);
        }
        printf("Successfuly parsed layer\n");
    }
    
} //namespace config_parser
} //namespace encoder