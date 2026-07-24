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

#include "services/layer_slicer.h"
#include "core/geometry_types.h"
#include "pipelines/pipe_data.h"
#include <cstdlib>

#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <cstddef>
#include <cmath>
#include <vector>

// ART, in the interest of performance, limits the maximum number of faces for a primitive to 128

namespace LayerSlicer {

    // inline arma::vec compute_distance(
    //     const arma::mat& A,
    //     const arma::mat& B,
    //     const arma::vec3& normal,
    //     double z_offset
    // ) {
    //     arma::vec W = B * normal;
    //     arma::vec D = A * W - z_offset;
    //     for (arma::uword i = 0; i < D.n_elem; i++) {
    //         if (std::abs(D[i]) < 1e-9) {
    //             D[i] = (D[i] >= 0.0) ? 1e-9 : -1e-9;
    //         }
    //     }

    //     return D;
    // }

    std::vector<Edge> compute_instance_intersection(
        const MeshInstance &inst,
        const MeshTemplate &tmpl,
        const arma::vec3 &plane_normal,
        double z_offset,
        const arma::vec3 &local_u,
        const arma::vec3 &local_v
    ) {
        std::vector<Edge> result;
        const double EPSILON = 1e-7;

        //Step 1: calculation distance to plane
        // std::size_t num_vertices = inst.control_points.n_rows;
        arma::mat actual_positions = tmpl.barycentric_coords * inst.control_points;
        std::size_t num_vertices = actual_positions.n_rows;
        std::vector<double> distances(num_vertices);

        for (std::size_t i = 0; i < num_vertices; i++) {
            // arma::vec3 pt = inst.control_points.row(i).t();
            arma::vec3 pt = actual_positions.row(i).t();
            double dist = arma::dot(pt, plane_normal) - z_offset;

            if (std::abs(dist) < EPSILON) dist = EPSILON;
            distances[i] = dist;   
        }

        // Step 2: Local basis projection
        arma::vec3 plane_origin = plane_normal * z_offset;

        // 3D -> 2D function
        auto project_to_2d = [&](const arma::vec3& p) -> arma::vec2 {
            arma::vec3 local_p = p - plane_origin;
            return arma::vec2{arma::dot(local_p, local_u), arma::dot(local_p, local_v)};
        };

        // Step 3: Edge generation
        std::size_t num_faces = tmpl.face_offsets.size() - 1;
        for (std::size_t f = 0; f < num_faces; f++) {
            std::vector<arma::vec3> face_intersections;

            std::size_t start_idx = tmpl.face_offsets[f];
            std::size_t end_idx = tmpl.face_offsets[f + 1];

            for (std::size_t e_idx = start_idx; e_idx < end_idx; e_idx++) {
                std::size_t edge_id = tmpl.face_edges[e_idx];
                const EdgeTopology& edge = tmpl.edges[edge_id];

                double d0 = distances[edge.v0];
                double d1 = distances[edge.v1];

                if (d0 * d1 < 0.0) {
                    // arma::vec3 p0 = inst.control_points.row(edge.v0).t();
                    // arma::vec3 p1 = inst.control_points.row(edge.v1).t();
                    arma::vec3 p0 = actual_positions.row(edge.v0).t();
                    arma::vec3 p1 = actual_positions.row(edge.v1).t();

                    double t = d0 / (d0 - d1);
                    arma::vec3 intersection = p0 + t * (p1 - p0);
                    face_intersections.push_back(intersection);
                }
            }

            // Step 4: 2D Edge generation
            if (face_intersections.size() == 2) {
                result.push_back({
                    project_to_2d(face_intersections[0]),
                    project_to_2d(face_intersections[1])
                });
            }
            else if (face_intersections.size() > 2) {
                for (std::size_t i = 0; i < face_intersections.size() - 1; i += 2) {
                    result.push_back({
                        project_to_2d(face_intersections[i]),
                        project_to_2d(face_intersections[i + 1])
                    });
                }
            }
        }

        return result;
    }

    void slice_layer(SliceContext &context, const FractalModel &model) {
        arma::vec3 normal = context.plane_normal.t();
        normal = arma::normalise(normal);

        arma::vec3 up = (std::abs(normal(2)) < 0.999) ? arma::vec3{0.0, 0.0, 1.0} : arma::vec3{1.0, 0.0, 0.0};

        arma::vec3 local_u = arma::normalise(arma::cross(up, normal));
        arma::vec3 local_v = arma::normalise(arma::cross(normal, local_u));

        const auto& automaton = model.get_automaton();

        for (const auto& inst : context.layer_meshes) {
            const auto& tmpl = automaton.get_reference_topology(inst.automaton_state);

            std::vector<Edge> inst_edges = compute_instance_intersection(inst, tmpl, normal, context.z_offset, local_u, local_v);
            context.flat_edges.insert(
                context.flat_edges.end(),
                inst_edges.begin(),
                inst_edges.end()
            );
        }

        context.layer_meshes.clear();
        context.layer_meshes.shrink_to_fit();
    }
    
} //namespace LayerSlicer