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

#ifndef PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H
#define PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H

#include "core/fractal_model.h"
#include "core/geometry_types.h"

#include <vector>

namespace MeshAssembler {
    // Callback function for progress reporting
    using ProgressCallback = std::function<void(float)>;
    
    /**
     * @brief Merges multiple mesh instances into a single mesh template for 3D rendering.
     * 
     * Consumes the instances vector (moves or clears it internally) to aggressively free RAM.
     * It uses std::execution::par internally to parallelize across cores.
     */
    MeshTemplate merge(
        std::vector<MeshInstance>&& instances,
        const FractalModel& model,
        ProgressCallback progress_cb = nullptr
    );
    
} // namespace MeshAssembler

#endif //PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H