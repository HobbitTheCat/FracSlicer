#ifndef PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H
#define PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H

#include "core/fractal_model.h"
#include "core/geometry_types.h"

#include <vector>

namespace MeshAssembler {
    // Callback function for progress reporting
    using ProgressCallback = std::function<void(float)>;
    
    /**
     * @brief Merges multiple mesh instances into a single mesh template for 3D rendering.
     * 
     * Consumes the instances vector (moves or clears it internally) to aggressively free RAM.
     * It uses std::execution::par internally to parallelize across cores.
     */
    MeshTemplate merge(
        std::vector<MeshInstance>&& instances,
        const FractalModel& model,
        ProgressCallback progress_cb = nullptr
    );
    
} // namespace MeshAssembler

#endif //PROJECT_FRAC_SLICER_MESH_ASSEMBLER_H