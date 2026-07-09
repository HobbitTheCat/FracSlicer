#include "services/layer_slicer.h"

#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <algorithm>
#include <cstddef>
#include <execution>
#include <cmath>
#include <numeric>
#include <vector>

// ART, in the interest of performance, limits the maximum number of faces for a primitive to 128

namespace LayerSlicer {

    inline arma::vec compute_distance(
        const arma::mat& A,
        const arma::mat& B,
        const arma::vec3& normal,
        double z_offset
    ) {
        arma::vec W = B * normal;
        arma::vec D = A * W - z_offset;
        for (arma::uword i = 0; i < D.n_elem; i++) {
            if (std::abs(D[i]) < 1e-9) {
                D[i] = (D[i] >= 0.0) ? 1e-9 : -1e-9;
            }
        }

        return D;
    }

    void compute_instance_intersection(
        const MeshInstance &inst,
        const MeshTemplate &tmpl, 
        const arma::vec3 &plane_normal, 
        const arma::vec3& local_U,
        const arma::vec3& local_V,
        double z_offset, 
        Edge* out_edge_ptr
    ) {
        const auto& A = tmpl.barycentric_coords;
        const auto& B = inst.control_points;

        arma::vec D = compute_distance(A, B, plane_normal, z_offset);
        
        std::size_t edge_write_idx = 0;
        const std::size_t num_faces = tmpl.face_offsets.size();

        arma::vec2 face_intersections[16];
        
        for (std::size_t f = 0; f < num_faces; ++f) {
            std::size_t start_edge = tmpl.face_offsets[f];
            std::size_t end_edge = (f + 1 < num_faces) ? tmpl.face_offsets[f+1] : tmpl.edges.size();
            
            std::size_t intersect_count = 0;

            for (std::size_t e = start_edge; e < end_edge; ++e) {
                const auto& edge = tmpl.edges[e];
                double d0 = D[edge.v0];
                double d1 = D[edge.v1];

                if ((d0 > 0.0) != (d1 > 0.0)) {
                    double t = d0 / (d0 - d1);
                    
                    arma::rowvec v0_3d = A.row(edge.v0) * B;
                    arma::rowvec v1_3d = A.row(edge.v1) * B;
                    arma::vec3 P = arma::trans(v0_3d + t * (v1_3d - v0_3d));

                    double u = arma::dot(P, local_U);
                    double v = arma::dot(P, local_V);

                    if (intersect_count < 16) {
                        face_intersections[intersect_count++] = {u, v};
                    }
                }
            }

            if (intersect_count == 2) {
                out_edge_ptr[edge_write_idx++] = { face_intersections[0], face_intersections[1] };
            }
        }
    }

    void slice_layer(SliceContext &context, const FractalModel &model) {
        if (context.layer_meshes.empty()) return;

        const auto& automaton = model.get_automaton();
        const std::size_t num_instances = context.layer_meshes.size();
        const double z_offset = context.z_offset;

        arma::vec3 n = arma::normalise(context.plane_normal).t();
        
        arma::vec3 abs_n = arma::abs(n);
        arma::vec3 up = (abs_n[2] < 0.999) ? arma::vec3{0.0, 0.0, 1.0} : arma::vec3{1.0, 0.0, 0.0};
        arma::vec3 local_U = arma::normalise(arma::cross(up, n));
        arma::vec3 local_V = arma::cross(n, local_U);

        std::vector<std::size_t> edge_counts(num_instances, 0);
        std::vector<std::size_t> indices(num_instances);
        std::iota(indices.begin(), indices.end(), 0);

        // Brief intersection count
        std::for_each(std::execution::par, indices.begin(), indices.end(), [&](std::size_t i) {
            const auto& inst = context.layer_meshes[i];
            const auto& tmpl = automaton.get_reference_topology(inst.automaton_state);
        
            if (tmpl.barycentric_coords.is_empty() || tmpl.edges.empty()) return;

            arma::vec D = compute_distance(tmpl.barycentric_coords, inst.control_points, n, z_offset);
                            
            std::size_t intersected_faces = 0;
            for (std::size_t f = 0; f < tmpl.face_offsets.size(); ++f) {
                std::size_t start = tmpl.face_offsets[f];
                std::size_t end = (f + 1 < tmpl.face_offsets.size()) ? tmpl.face_offsets[f+1] : tmpl.edges.size();
                
                std::size_t cross_count = 0;
                for (std::size_t e = start; e < end; ++e) {
                    if ((D[tmpl.edges[e].v0] > 0.0) != (D[tmpl.edges[e].v1] > 0.0)) {
                        cross_count++;
                    }
                }
                if (cross_count == 2) intersected_faces++;
            }
            edge_counts[i] = intersected_faces;
        });

        // Memory allocation
        std::vector<std::size_t> edge_offsets(num_instances + 1, 0);
        std::inclusive_scan(std::execution::par, edge_counts.begin(), edge_counts.end(), edge_offsets.begin() + 1);

        std::size_t total_edges = edge_offsets.back();
        context.flat_edges.resize(total_edges);

        // Edge generation in buffer + memory dealloc
        std::for_each(std::execution::par, indices.begin(), indices.end(), [&](std::size_t i){
            if (edge_counts[i] == 0) {
                context.layer_meshes[i].control_points.reset();
                return;
            }

            auto& inst = context.layer_meshes[i];
            const auto& tmpl = automaton.get_reference_topology(inst.automaton_state);

            std::size_t write_offset = edge_offsets[i];
            compute_instance_intersection(inst, tmpl, n, local_U, local_V, z_offset, &context.flat_edges[write_offset]);

            // inst.control_points.reset(); //TODO подумать нужно ли оно тут или все же нет так как edge весят сильно меньше полигонов. и в паяти на оба хватит места
        });

        context.layer_meshes.clear();
        context.layer_meshes.shrink_to_fit();
    }
    
} //namespace LayerSlicer