#pragma once

#include "core/geometry_types.h"
#include "encoder/layer_encoder.h"

#include <vector>

struct Edge {
    arma::vec2 p0; // intersection plane basis coords
    arma::vec2 p1;
};

struct SliceContext {       // TODO найти место получше для этого struct если оно есть
    int layer_id = 0;
    double z_offset = 0.0;
    arma::rowvec plane_normal;

    std::vector<MeshInstance> layer_meshes;
    std::vector<Edge> flat_edges;
    
    encoder::LayerEncoder layer_encoder;
    std::vector<uint8_t> preview_buffer;
};