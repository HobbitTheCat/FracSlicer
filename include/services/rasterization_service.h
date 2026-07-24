/*
 * Copyright (C) 2026 Basilisk
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

#ifndef PROJECT_FRAC_SLICER_RASTERIZATION_H
#define PROJECT_FRAC_SLICER_RASTERIZATION_H

#include "pipelines/pipe_data.h"
#include "core/printer.h"

namespace RasterizationService {

    struct PreviewSettings {
        int width = 1920;
        int height = 1080;
        bool enabled = true;
    };
    
    struct ScanlineEdge {
        int y_max;
        double x;
        double dx_dy;
    };

    /**
     * @brief Converts 2D continuous edges into compressed pixel rows.
     * 
     * This function processes context.flat_edges, translates them to pixel space
     * using the Printer settings, performs scanline filling, populates
     */
    void rasterize_layer(SliceContext& context, const Printer& printer, const PreviewSettings& preview);

} // namespace RasterizationService

#endif //PROJECT_FRAC_SLICER_RASTERIZATION_H