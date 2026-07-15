#ifndef PROJECT_FRAC_SLICER_INTERACTIVE_VIEWER_H
#define PROJECT_FRAC_SLICER_INTERACTIVE_VIEWER_H

#include "core/fractal_model.h"
#include "renderer/ui_structures.h"  // TODO come up with another way to transfer the plane

#include <future>
#include <atomic>
#include <memory>

#include "services/geometry_converter.h"

class InteractiveViewer {
private:
    std::future<RenderGeometry> task_future;  
    std::atomic<bool> is_busy{false};
    std::atomic<float> progress{0.0f};
    std::atomic<bool> should_cancel{false};

    // std::mutex queue_mutex; ART it is possible to display intermediate iterations of the fractal
    // std::queue<MeshTemplate> intermediate_queue; same as in slicing_pipeline
public:
    InteractiveViewer() = default;
    ~InteractiveViewer();

    void start_generation(std::shared_ptr<const FractalModel> model, int iteration, const PlaneSettings& plane);

    bool update(RenderGeometry& out_geometry);

    bool is_working() const {return this->is_busy.load();}
    float get_progress() const {return this->progress.load();}

    void cancel();
};

#endif //PROJECT_FRAC_SLICER_INTERACTIVE_VIEWER_H