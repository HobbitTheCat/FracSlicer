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

#ifndef PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H
#define PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H

#include "core/fractal_model.h"
#include "core/geometry_types.h"

#include <vector>
#include <functional>

namespace FractalGenerator {

    // Callback function for progress reporting
    using ProgressCallback = std::function<void(float)>;
    // A predicate function to stop when necessary
    using CancelPredicate = std::function<bool()>;

    /**
    * @brief Generates fractal instances on the CPU.
    * 
    * This function is completely synchronous and runs on the caller's thread.
    * It uses std::execution::par internally to parallelize across cores.
    */
    
    std::vector<MeshInstance> evaluate(
        const FractalModel& model,
        int target_iteration,
        CullingFilter filter = nullptr,
        ProgressCallback progress_cb = nullptr,
        CancelPredicate should_cancel = nullptr
    );

    /**
     * @brief Calculating a safe number of iterations based on available memory.
     * 
     * TODO: Move the function to a separate module called MemoryBudgetService
     */
    int estimate_max_iteration(
        const FractalModel& model,
        std::size_t memory_budget_bytes = 8ULL * 1024 * 1024 * 1024 // 8Gb
    );
    
} // namespace FractalGenerator

#endif //PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H