#include "services/geometry_converter.h"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <execution>
#include <numeric>
#include <vector>

namespace GeometryConverter {

    RenderGeometry convert_to_render_geometry(MeshTemplate &&mesh) {
        if (mesh.barycentric_coords.is_empty() || mesh.polygons.is_empty()) {
            printf("GeometryConverter: mesh does not have barycentric coords");
            return RenderGeometry{};
        }

        RenderGeometry result;
        size_t num_vertices = mesh.barycentric_coords.n_rows;
        size_t num_indices = mesh.polygons.n_elem;

        result.vertices.resize(num_vertices * 3);
        result.indices.resize(num_indices);

        std::vector<std::size_t> vertex_indices(num_vertices);
        std::iota(vertex_indices.begin(), vertex_indices.end(), 0);

        std::for_each(std::execution::par, vertex_indices.begin(), vertex_indices.end(), [&](std::size_t i) {
            result.vertices[i * 3 + 0] = static_cast<float>(mesh.barycentric_coords(i, 0));
            result.vertices[i * 3 + 1] = static_cast<float>(mesh.barycentric_coords(i, 1));
            result.vertices[i * 3 + 2] = static_cast<float>(mesh.barycentric_coords(i, 2));
        });

        if (sizeof(arma::uword) == sizeof(unsigned int)) {
            std::memcpy(result.indices.data(), mesh.polygons.memptr(), num_indices * sizeof(unsigned int));
        } else {
            std::copy(mesh.polygons.begin(), mesh.polygons.end(), result.indices.begin());
        }
        
        mesh.barycentric_coords.reset();
        mesh.polygons.reset();
        
        return result;
    }
    
} //namespace GeometryConverter