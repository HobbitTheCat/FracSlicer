#pragma once

#include "binary_writer.h"
#include "header.h"
#include "layer.h"
#include "layer_encoder.h"

#include "core/printer.h"


namespace encoder {
    class GooEncoder {
    private:
        void write_header_internal();

        BinaryWriter writer;
        Header header;

        Layer default_bottom;
        Layer default_normal;
        
        uint32_t layer_written{0};
        bool is_closed{false};

    public:
        GooEncoder(const std::string& config_json_path, const std::string& output_goo_path);

        ~GooEncoder();

        Printer get_printer(double scale_1_to_mm) const;

        void write_layer(const Layer& layer_param, const LayerEncoder& layer_data);

        void write_layer(float z_offset, const LayerEncoder& layer_data);
        
        void close();

        const Header& get_header() const {return this->header;}
    };
    
} // namespace encoder