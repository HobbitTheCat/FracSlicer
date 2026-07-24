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

#include "include/services/fractal_generator.h"

#include <algorithm>
#include <cstddef>
#include <execution>
#include <iterator>
#include <numeric>
#include <vector>

namespace FractalGenerator {
    
    std::vector<MeshInstance> evaluate(
        const FractalModel &model, 
        int target_iteration,
        CullingFilter filter,
        ProgressCallback progress_cb,
        CancelPredicate should_cancel
    ) {
        std::vector<MeshInstance> current_meshes = model.get_initial_meshes();
    
        for (int iter = 0; iter < target_iteration; iter++) {
            // cancel signal
            if (should_cancel && should_cancel()) {
                current_meshes.clear();
                current_meshes.shrink_to_fit();
                return {};
            }
    
            if (current_meshes.empty()) break;
    
            const std::size_t current_size = current_meshes.size();
            std::vector<std::vector<MeshInstance>> thread_results(current_meshes.size());
    
            std::vector<std::size_t> indices(current_size);
            std::iota(indices.begin(), indices.end(), 0);
    
            std::for_each(std::execution::par, indices.begin(), indices.end(),
                [&](std::size_t i) {
                    model.get_automaton().evaluate(current_meshes[i], thread_results[i], filter);
                }
            );
    
            current_meshes.clear();
            for (auto& res : thread_results) {
                current_meshes.insert(
                    current_meshes.end(),
                    std::make_move_iterator(res.begin()),
                    std::make_move_iterator(res.end())
                );
            }
    
            if (progress_cb) progress_cb(static_cast<float>(iter + 1) / target_iteration); //TODO: There is an opportunity here to improve the user experience by calculating interest more accurately.
        }
    
        return current_meshes;
    }
    
    int estimate_max_iteration(const FractalModel &model, std::size_t memory_budget_bytes) {
        std::size_t initial_meshes_size = 0;
        for (const auto& mesh : model.get_initial_meshes())
            initial_meshes_size += (sizeof(MeshInstance) + mesh.control_points.n_elem * sizeof(double));
        if (initial_meshes_size == 0) return 0;
        std::size_t transitions = model.get_automaton().get_transition_number();
        if (transitions < 2) return 0;
        if (initial_meshes_size >= memory_budget_bytes) return 0;
        double available_multiplier = static_cast<double>(memory_budget_bytes) / initial_meshes_size;
        double max_iter = std::log(available_multiplier) / std::log(transitions);
        return static_cast<int>(std::floor(max_iter));
    }
    
} // namespace FractalGenerator