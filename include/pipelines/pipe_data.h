#pragma once

#include "core/geometry_types.h"
#include "encoder/layer_encoder.h"

#include <array>
#include <vector>

struct Edge {
    arma::vec2 p0; // intersection plane basis coords
    arma::vec2 p1;
};

struct SliceContext {
    int layer_id = 0;
    double z_offset = 0.0;
    arma::rowvec plane_normal;

    std::vector<MeshInstance> layer_meshes;

    std::vector<Edge> flat_edges; // TODO delete
    std::vector<std::array<double, 2>> layer_vertices;
    std::vector<uint32_t> layer_indices;
    
    encoder::LayerEncoder layer_encoder;
    std::vector<uint8_t> preview_buffer;
};