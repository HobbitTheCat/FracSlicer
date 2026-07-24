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

#ifndef PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H
#define PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H

#include "core/geometry_types.h"
#include <vector>

struct RenderGeometry {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

namespace GeometryConverter {

    /**
     * @brief Converts MeshTemplate into flat vectors for OpenGL.
     * This operation runs in parallel using std::execution::par to speed up the float conversion.
     * 
     */
    RenderGeometry convert_to_render_geometry(MeshTemplate&& mesh);
    
} // namespace GeometryConverter

#endif //PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H