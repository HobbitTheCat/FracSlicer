#include "services/mesh_assebler.h"

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <execution>

namespace MeshAssembler {
    // TODO проверить можно ли освобождать instances прямо во время вычислений что бы не все в конце а сразу по ходу вычислений
    MeshTemplate merge(
        std::vector<MeshInstance>&& instances,
        const FractalModel& model, 
        ProgressCallback progress_cb
    ) {
        if (instances.empty()) return MeshTemplate{};
    
        const auto& automaton = model.get_automaton();
        std::size_t num_instances = instances.size();
    
        std::vector<std::size_t> v_offsets(num_instances);
        std::vector<std::size_t> p_offsets(num_instances);
        std::vector<std::size_t> v_counts(num_instances);
        std::vector<std::size_t> p_counts(num_instances);
    
        std::vector<std::size_t> indices(num_instances);
        std::iota(indices.begin(), indices.end(), 0);
    
        std::for_each(std::execution::par, indices.begin(), indices.end(), [&](std::size_t i) {
            const auto& tmpl = automaton.get_reference_topology(instances[i].automaton_state);
            v_counts[i] = tmpl.barycentric_coords.n_rows;
            p_counts[i] = tmpl.polygons.n_elem;
        });
    
        if (progress_cb) progress_cb(0.1);
    
        std::inclusive_scan(std::execution::par, v_counts.begin(), v_counts.end(), v_offsets.begin());
        std::inclusive_scan(std::execution::par, p_counts.begin(), p_counts.end(), p_offsets.begin());
        
        std::size_t total_vertices = v_offsets.back();
        std::size_t total_polygons = p_offsets.back();
    
        MeshTemplate result;
        result.barycentric_coords.set_size(total_vertices, 3);
        result.polygons.set_size(total_polygons);
    
        if (progress_cb) progress_cb(0.3f);
    
        std::for_each(std::execution::par, indices.begin(), indices.end(), [&](std::size_t i) {
            const auto& inst = instances[i];
            const auto& tmpl = automaton.get_reference_topology(inst.automaton_state);
    
            if (v_counts[i] == 0) return;
    
            std::size_t current_v = v_offsets[i] - v_counts[i];
            std::size_t current_p = p_offsets[i] - p_counts[i];
    
            const auto& A = tmpl.barycentric_coords;
            const auto& B = inst.control_points;
    
            for (std::size_t row = 0; row < v_counts[i]; row++) {
                for (std::size_t col = 0; col < 3; col++) {
                    double val = 0.0;
                    for (std::size_t k = 0; k < A.n_cols; k++) {
                        val += A(row, k) * B(k, col);
                    }
                    result.barycentric_coords(current_v + row, col) = val;
                }
            }
    
            if (p_counts[i] > 0) 
                result.polygons.subvec(current_p, current_p + p_counts[i] - 1) = tmpl.polygons + current_v;
        });
    
        if (progress_cb) progress_cb(0.9f);
    
        instances.clear();
        instances.shrink_to_fit();
    
        if (progress_cb) progress_cb(1.0f);
        return result;
    }

} //namespace MeshAssembler