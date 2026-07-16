#include "services/polygon_builder.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

#include "earcut/earcut.hpp"

namespace mapbox { namespace util {

template <> struct nth<0, std::array<double, 2>> { static auto get(const std::array<double, 2>& t) { return t[0]; } };
template <> struct nth<1, std::array<double, 2>> { static auto get(const std::array<double, 2>& t) { return t[1]; } };

}} // namespace mapbox::util

namespace PolygonBuilder {

std::vector<Polygon2D> build_closed_polygons(
    const MeshInstance &inst,
    const MeshTemplate &tmpl,
    const arma::vec3 &plane_normal, 
    double z_offset,
    const arma::vec3 &local_u, 
    const arma::vec3 &local_v
) {
    const double EPSILON = 1e-7;
    std::vector<Polygon2D> result_polygons;

    // Step 1: Calculation of distance from each vertex to plane
    arma::mat actual_positions = tmpl.barycentric_coords * inst.control_points;
    
    std::size_t num_vertices = actual_positions.n_rows;
    std::vector<double> distances(num_vertices);

    for (std::size_t i = 0; i < num_vertices; i++) {
        arma::vec3 pt = actual_positions.row(i).t();
        double dist = arma::dot(pt, plane_normal) - z_offset;

        if (std::abs(dist) < EPSILON) dist = (dist >= 0.0) ? EPSILON : -EPSILON;
        distances[i] = dist;
    }

    arma::vec3 plane_origin = plane_normal * z_offset;
    auto project_to_2d = [&](const arma::vec3& p) -> std::array<double, 2> {
        arma::vec3 local_p = p - plane_origin;
        return {arma::dot(local_p, local_u), arma::dot(local_p, local_v)};
    };

    std::vector<std::array<double, 2>> points_2d;
    std::unordered_map<std::size_t, std::vector<std::size_t>> face_to_points;

    for (const auto& edge : tmpl.edges) {
        double d0 = distances[edge.v0];
        double d1 = distances[edge.v1];

        if (d0 * d1 < 0.0) {
            arma::vec3 p0 = actual_positions.row(edge.v0).t();
            arma::vec3 p1 = actual_positions.row(edge.v1).t();

            double t = d0 / (d0 - d1);
            arma::vec3 intersection_3d = p0 + t * (p1 - p0);

            std::size_t current_point_id = points_2d.size();
            points_2d.push_back(project_to_2d(intersection_3d));

            face_to_points[edge.f0].push_back(current_point_id);
            face_to_points[edge.f1].push_back(current_point_id);
        }
    }

    std::vector<std::vector<std::size_t>> adjacency_list(points_2d.size());
    std::size_t skipped_complex_faces = 0;

    for (const auto& [face_id, points_ids] : face_to_points) {
        if (points_ids.size() == 2) {
            std::size_t pA = points_ids[0];
            std::size_t pB = points_ids[1];
            adjacency_list[pA].push_back(pB);
            adjacency_list[pB].push_back(pA);
        } else if (points_ids.size() > 2) {
            std::vector<std::size_t> sorted_points = points_ids;
            std::sort(sorted_points.begin(), sorted_points.end(), [&](std::size_t a, std::size_t b) {
                const auto& pA = points_2d[a];
                const auto& pB = points_2d[b];
                if (std::abs(pA[0] - pB[0]) > EPSILON) return pA[0] < pB[0];
                return pA[1] < pB[1];
            });
            for (std::size_t i = 0; i + 1 < sorted_points.size(); i += 2) {
                std::size_t pA = sorted_points[i];
                std::size_t pB = sorted_points[i+1];
                adjacency_list[pA].push_back(pB);
                adjacency_list[pB].push_back(pA);
            }
        } else skipped_complex_faces++;
    }

    std::vector<bool> visited(points_2d.size(), false);
    std::size_t unclosed_loops = 0;

    for (std::size_t i = 0; i < points_2d.size(); i++) {
        if (visited[i] || adjacency_list[i].empty()) continue;

        Polygon2D current_polygon;
        std::size_t current_node = i;
        std::size_t start_node = i;
        std::size_t prev_node = std::numeric_limits<std::size_t>::max();

        while (!visited[current_node]) {
            visited[current_node] = true;
            current_polygon.push_back(points_2d[current_node]);

            std::size_t next_node = std::numeric_limits<std::size_t>::max();

            for (std::size_t neighbor : adjacency_list[current_node]) {
                if (neighbor != prev_node && !visited[neighbor]) {
                    next_node = neighbor;
                    break;
                }
            }

            if (next_node != std::numeric_limits<std::size_t>::max()) {
                prev_node = current_node;
                current_node = next_node;
            } else {
                break;
            }
        }

        if (current_polygon.size() >= 3) {
            bool is_closed = false;
            for (std::size_t neighbor : adjacency_list[current_node]) {
                if (neighbor == start_node) {
                    is_closed = true;
                    break;
                }
            }
            if (!is_closed) unclosed_loops++;
            
            result_polygons.push_back(current_polygon);
        }
    }

    if (unclosed_loops > 0 || skipped_complex_faces > 0) {
        std::cout << "[WARNING] PolygonBuilder: " << unclosed_loops << " unclosed loops, " 
                  << skipped_complex_faces << " singular intersections detected.\n";
    }

    return result_polygons;
}

void triangulate_polygon(
    const Polygon2D &polygon,
    std::vector<std::array<double, 2>>& global_vertices,
    std::vector<uint32_t>& global_indices 
) {
    if (polygon.size() < 3) return;
    
    uint32_t index_offset = static_cast<uint32_t>(global_vertices.size());

    global_vertices.insert(global_vertices.end(), polygon.begin(), polygon.end());

    std::vector<Polygon2D> earcut_input = { polygon };
    std::vector<uint32_t> local_indices = mapbox::earcut<uint32_t>(earcut_input);

    global_indices.reserve(global_indices.size() + local_indices.size());
    for (uint32_t idx : local_indices) {
        global_indices.push_back(idx + index_offset);
    }
}

void slice_layer(SliceContext& context, const FractalModel& model) {
    arma::vec3 normal = context.plane_normal.t();
    normal = arma::normalise(normal);

    arma::vec3 up = (std::abs(normal(2)) < 0.999) ? arma::vec3{0.0, 0.0, 1.0} : arma::vec3{1.0, 0.0, 0.0};

    arma::vec3 local_u = arma::normalise(arma::cross(up, normal));
    arma::vec3 local_v = arma::normalise(arma::cross(normal, local_u));

    const auto& automaton = model.get_automaton();

    for (const auto& inst : context.layer_meshes) {
        const auto& tmpl = automaton.get_reference_topology(inst.automaton_state);

        auto result_polygons = build_closed_polygons(inst, tmpl, normal, context.z_offset, local_u, local_v);

        for (const auto& poly : result_polygons) {
            triangulate_polygon(poly, context.layer_vertices, context.layer_indices);
        }
    }

    context.layer_meshes.clear();
    context.layer_meshes.shrink_to_fit();
}

} //namespace PolygonBuilder