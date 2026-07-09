#ifndef PROJECT_FRAC_SLICER_LAYER_SLICER_H
#define PROJECT_FRAC_SLICER_LAYER_SLICER_H

#include "core/fractal_model.h"
#include "pipelines/pipe_data.h"
#include <vector>

namespace LayerSlicer {

    /**
    * @brief Slices the geometry provided in the SliceContext for its specific Z-offset.
    * 
    * This function processes context->layer_meshes in parallel, computes plane intersections,
    * CLEARS context->layer_meshes to free up memory immediately.
    */
    void slice_layer(SliceContext& context, const FractalModel& model);

    /**
     * @brief Computes plane intersection for a single instance and stores vertives in a buffer
     * 
     */
    std::vector<Edge> compute_instance_intersection(
        const MeshInstance &inst,
        const MeshTemplate &tmpl, 
        const arma::vec3 &plane_normal, 
        double z_offset,
        const arma::vec3& local_u,
        const arma::vec3& local_v
    );
    
} // namespace LayerSlicer

#endif //PROJECT_FRAC_SLICER_LAYER_SLICER_H