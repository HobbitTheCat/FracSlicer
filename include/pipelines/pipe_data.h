#pragma once

#include "core/geometry_types.h"

#include <vector>

struct Edge {
    arma::vec2 p0; // intersection plane basis coords
    arma::vec2 p1;
};

struct PixelSpan {
    uint32_t length;
    uint8_t value;
};

struct SliceContext {
    int layer_id = 0;
    double z_offset = 0.0;
    arma::rowvec plane_normal;

    std::vector<MeshInstance> layer_meshes;
    std::vector<Edge> flat_edges;
    
    std::vector<PixelSpan> pixel_spans;
    std::vector<uint8_t> preview_buffer;
};