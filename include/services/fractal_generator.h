#ifndef PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H
#define PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H

#include "core/fractal_model.h"
#include "core/geometry_types.h"

#include <vector>
#include <functional>

namespace FractalGenerator {

    // Callback function for progress reporting
    using ProgressCallback = std::function<void(float)>;
    // A predicate function to stop when necessary
    using CancelPredicate = std::function<bool()>;

    /**
    * @brief Generates fractal instances on the CPU.
    * 
    * This function is completely synchronous and runs on the caller's thread.
    * It uses std::execution::par internally to parallelize across cores.
    */
    
    std::vector<MeshInstance> evaluate(
        const FractalModel& model,
        int target_iteration,
        CullingFilter filter = nullptr,
        ProgressCallback progress_cb = nullptr,
        CancelPredicate should_cancel = nullptr
    );

    /**
     * @brief Calculating a safe number of iterations based on available memory.
     * 
     * TODO: Move the function to a separate module called MemoryBudgetService
     */
    int estimate_max_iteration(
        const FractalModel& model,
        std::size_t memory_budget_bytes = 8ULL * 1024 * 1024 * 1024 // 8Gb
    );
    
} // namespace FractalGenerator

#endif //PROJECT_FRAC_SLICER_FRACTAL_GENERATOR_H