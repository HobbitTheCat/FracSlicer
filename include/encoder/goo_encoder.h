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