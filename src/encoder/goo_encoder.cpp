#include "encoder/goo_encoder.h"
#include "encoder/config_parser.h"
#include "encoder/constants.h"

#include <stdexcept>

namespace encoder {

GooEncoder::GooEncoder(const std::string& config_json_path, const std::string& output_goo_path) 
    : writer(output_goo_path),
      header(config_parser::parse_header(config_json_path)
){
    config_parser::parse_layer(config_json_path, this->default_bottom, this->default_normal);
    this->write_header_internal();
}

GooEncoder::~GooEncoder() {
    try { this->close();
    } catch (...) {}
}

Printer GooEncoder::get_printer(double scale_1_to_mm) const {
    Printer::Resolution res {
        static_cast<int>(this->header.x_resolution),
        static_cast<int>(this->header.y_resolution)
    };

    Printer::PrintableArea area {
        static_cast<double>(this->header.x_size),
        static_cast<double>(this->header.y_size)
    };

    double layer_height_mm = static_cast<double>(this->header.layer_thickness);

    return Printer(res, area, layer_height_mm, scale_1_to_mm);
}

void GooEncoder::write_header_internal() {
    // 0. version
    this->writer.write_bytes(this->header.version.data);
    
    // 1. Magic Tag
    this->writer.write_bytes(constants::MAGIC_TAG);
    
    // 2-7. Текстовая информация
    this->writer.write_bytes(this->header.software_info.data);
    this->writer.write_bytes(this->header.software_version.data);
    this->writer.write_bytes(this->header.file_time.data);
    this->writer.write_bytes(this->header.printer_name.data);
    this->writer.write_bytes(this->header.printer_type.data);
    this->writer.write_bytes(this->header.resin_profile_name.data);

    // 8-10. Настройки сглаживания и размытия
    this->writer.write_u16_be(this->header.anti_aliasing_level);
    this->writer.write_u16_be(this->header.grey_level);
    this->writer.write_u16_be(this->header.blur_level);

    // 11-14. Превью-изображения и разделители
    this->writer.write_bytes(this->header.small_preview.data);
    this->writer.write_bytes(constants::DELIMITER);
    this->writer.write_bytes(this->header.big_preview.data);
    this->writer.write_bytes(constants::DELIMITER);

    // 15-19. Метаданные о разрешении и количестве слоев
    this->writer.write_u32_be(this->header.total_layers);
    this->writer.write_u16_be(this->header.x_resolution);
    this->writer.write_u16_be(this->header.y_resolution);
    this->writer.write_bool(this->header.x_mirror);
    this->writer.write_bool(this->header.y_mirror);

    // 20-23. Геометрия платформы
    this->writer.write_f32_be(this->header.x_size);
    this->writer.write_f32_be(this->header.y_size);
    this->writer.write_f32_be(this->header.z_size);
    this->writer.write_f32_be(this->header.layer_thickness);

    // 24-32. Временные задержки и параметры экспозиции
    this->writer.write_f32_be(this->header.exposure_time);
    this->writer.write_bool(this->header.exposure_delay_mode == ExposureDelayMode::StaticTime);
    this->writer.write_f32_be(this->header.turn_off_time);
    this->writer.write_f32_be(this->header.bottom_before_lift_time);
    this->writer.write_f32_be(this->header.bottom_after_lift_time);
    this->writer.write_f32_be(this->header.bottom_after_retract_time);
    this->writer.write_f32_be(this->header.before_lift_time);
    this->writer.write_f32_be(this->header.after_lift_time);
    this->writer.write_f32_be(this->header.after_retract_time);

    // 33-42. Скорости и дистанции (основные)
    this->writer.write_f32_be(this->header.bottom_exposure_time);
    this->writer.write_u32_be(this->header.bottom_layers);
    this->writer.write_f32_be(this->header.bottom_lift_distance);
    this->writer.write_f32_be(this->header.bottom_lift_speed);
    this->writer.write_f32_be(this->header.lift_distance);
    this->writer.write_f32_be(this->header.lift_speed);
    this->writer.write_f32_be(this->header.bottom_retract_distance);
    this->writer.write_f32_be(this->header.bottom_retract_speed);
    this->writer.write_f32_be(this->header.retract_distance);
    this->writer.write_f32_be(this->header.retract_speed);

    // 43-50. Скорости и дистанции (вторая стадия движения)
    this->writer.write_f32_be(this->header.bottom_second_lift_distance);
    this->writer.write_f32_be(this->header.bottom_second_lift_speed);
    this->writer.write_f32_be(this->header.second_lift_distance);
    this->writer.write_f32_be(this->header.second_lift_speed);
    this->writer.write_f32_be(this->header.bottom_second_retract_distance);
    this->writer.write_f32_be(this->header.bottom_second_retract_speed);
    this->writer.write_f32_be(this->header.second_retract_distance);
    this->writer.write_f32_be(this->header.second_retract_speed);

    // 51-53. Мощность света и флаг Advanced Mode
    this->writer.write_u16_be(this->header.bottom_light_pwm);
    this->writer.write_u16_be(this->header.light_pwm);
    this->writer.write_bool(this->header.advance_mode);

    // 54-58. Статистика печати
    this->writer.write_u32_be(this->header.printing_time);
    this->writer.write_f32_be(this->header.total_volume);
    this->writer.write_f32_be(this->header.total_weight);
    this->writer.write_f32_be(this->header.total_price);
    this->writer.write_bytes(this->header.price_unit.data);

    // 59. Адрес начала контента (Offset of LayerContent)
    this->writer.write_u32_be(static_cast<uint32_t>(constants::HEADER_SIZE));
    
    // 60-61. Настройки серого и переходных слоев
    this->writer.write_bool(this->header.grey_scale_level);
    this->writer.write_u16_be(this->header.transition_layers);
}

void GooEncoder::write_layer(float z_offset, const LayerEncoder& layer_data) {
    Layer current_params = (this->layer_written < header.bottom_layers) ? default_bottom : default_normal;
    current_params.layer_position_z = z_offset;
    this->write_layer(current_params, layer_data);
}

void GooEncoder::write_layer(const Layer& layer_params, const LayerEncoder& layer_data) {
    if (this->layer_written >= this->header.total_layers) {
        throw std::runtime_error("Attempted to write more layers than specified in total_layers");
    }

    this->writer.write_u16_be(layer_params.pause ? 1 : 0);
    this->writer.write_f32_be(layer_params.pause_position_z);
    this->writer.write_f32_be(layer_params.layer_position_z);
    this->writer.write_f32_be(layer_params.layer_exposure_time);
    this->writer.write_f32_be(layer_params.layer_off_time);
    this->writer.write_f32_be(layer_params.before_lift_time);
    this->writer.write_f32_be(layer_params.after_lift_time);
    this->writer.write_f32_be(layer_params.after_retract_time);
    this->writer.write_f32_be(layer_params.lift_distance);
    this->writer.write_f32_be(layer_params.lift_speed);
    this->writer.write_f32_be(layer_params.second_lift_distance);
    this->writer.write_f32_be(layer_params.second_lift_speed);
    this->writer.write_f32_be(layer_params.retract_distance);
    this->writer.write_f32_be(layer_params.retract_speed);
    this->writer.write_f32_be(layer_params.second_retract_distance);
    this->writer.write_f32_be(layer_params.second_retract_speed);
    this->writer.write_u16_be(layer_params.light_pwm);

    this->writer.write_bytes(constants::DELIMITER);

    const auto& compressed_data = layer_data.get_data();
    uint32_t data_size = static_cast<uint32_t>(compressed_data.size()) + 2; 
    this->writer.write_u32_be(data_size);

    // 3. Image data
    this->writer.write_u8(0x55); // Magic number at the begin of this field
    this->writer.write_bytes(compressed_data);
    this->writer.write_u8(layer_data.get_checksum()); // Checksum at the end

    this->writer.write_bytes(constants::DELIMITER);

    this->layer_written++;
}

void GooEncoder::close() {
    if (!this->is_closed) {
        this->writer.write_bytes(constants::ENDING_STRING);

        this->header.total_layers = this->layer_written;

        this->writer.seek_to_start();
        this->write_header_internal();
        
        this->is_closed = true;
    }
}
    
} //namespace encoder