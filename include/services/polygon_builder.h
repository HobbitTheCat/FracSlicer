#pragma once

#include "core/fractal_model.h"
#include "pipelines/pipe_data.h"
#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <array>
#include <vector>
#include "core/geometry_types.h"

namespace PolygonBuilder {
    struct TriangulationResult {
        std::vector<std::array<double, 2>> vertices;
        std::vector<uint32_t> indices;
    };
    
    using Polygon2D = std::vector<std::array<double, 2>>;

    std::vector<Polygon2D> build_closed_polygons(
        const MeshInstance& inst,
        const MeshTemplate& tmpl,
        const arma::vec3& plane_normal,
        double z_offset,
        const arma::vec3& local_u,
        const arma::vec3& local_v
    );

    void triangulate_polygon(
        const Polygon2D &polygon,
        std::vector<std::array<double, 2>>& global_vertices,
        std::vector<uint32_t>& global_indices 
    );

    void slice_layer(SliceContext& context, const FractalModel& model);
} //namespace PolygoneBuilder