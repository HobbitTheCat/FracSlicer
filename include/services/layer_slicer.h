#ifndef PROJECT_FRAC_SLICER_LAYER_SLICER_H
#define PROJECT_FRAC_SLICER_LAYER_SLICER_H

#include "core/fractal_model.h"
#include "pipelines/pipe_data.h"

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
     * TODO: Avoiding any interanl allocations or temporary structures
     */
    void compute_instance_intersection(
        const MeshInstance &inst,
        const MeshTemplate &tmpl, 
        const arma::vec3 &plane_normal, 
        const arma::vec3& local_U,
        const arma::vec3& local_V,
        double z_offset, 
        Edge* out_edge_ptr
    );
    
} // namespace LayerSlicer

#endif //PROJECT_FRAC_SLICER_LAYER_SLICER_H