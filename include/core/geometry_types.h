#ifndef PROJECT_FRAC_SLICER_LOCAL_MESH_H
#define PROJECT_FRAC_SLICER_LOCAL_MESH_H

#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include <armadillo>
#include <vector>
#include <cstddef>

struct EdgeTopology {
    std::size_t v0, v1; // First and second points
    std::size_t f0, f1; // First and second faces
};

// ART: The choice of this data structure may not be optimal if there are other structures better suited to solving the problem 
//      of finding the polygon resulting from the intersection of a shape with a plane

struct MeshTemplate {
    arma::mat barycentric_coords;
    std::vector<EdgeTopology> edges;

    std::vector<std::size_t> face_offsets;
    std::vector<std::size_t> face_edges;
    
    arma::uvec polygons;
};

struct MeshInstance {
    arma::mat control_points;
    std::size_t automaton_state;
};

using CullingFilter = std::function<bool(const MeshInstance&)>; // этому тут тоже не место

#endif //PROJECT_FRAC_SLICER_LOCAL_MESH_H