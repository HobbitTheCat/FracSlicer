#include "pipelines/interactive_view.h"
#include "core/fractal_filters.h"
#include "services/fractal_generator.h"
#include "services/mesh_assebler.h"

#include <chrono>
#include <future>
#include <utility>

InteractiveViewer::~InteractiveViewer() {
    this->cancel();
}

void InteractiveViewer::start_generation(std::shared_ptr<const FractalModel> model, int iteration, const PlaneSettings& plane) {
    if (is_busy.load()) return;

    is_busy.store(true);
    progress.store(0.0f);
    should_cancel.store(false);

    this->task_future = std::async(std::launch::async, [model, iteration, plane, this](){
        auto progress_callback = [this](float p) {this->progress.store(p * 0.8f);};
        auto cancel_predicate = [this]() {return this->should_cancel.load();};
        
        CullingFilter filter = nullptr;
        if (plane.enabled) filter = FractalFilters::plane_intersection({plane.normal.x, plane.normal.y, plane.normal.z}, -plane.z_offset, plane.cut_mode);

        // Step 1: Instance generation;
        auto instances = FractalGenerator::evaluate(*model, iteration, filter, progress_callback, cancel_predicate);
        if (this->should_cancel.load()) return RenderGeometry{};

        // Step 2: Mesh assembling
        this->progress.store(0.8f);
        auto final_mesh = MeshAssembler::merge(std::move(instances), *model);
        if (this->should_cancel.load()) return RenderGeometry{};
        this->progress.store(9.0f);

        // Step 3: Data conversion
        auto render_geo = GeometryConverter::convert_to_render_geometry(std::move(final_mesh));

        this->progress.store(1.0f);
        return render_geo;
    });
}

bool InteractiveViewer::update(RenderGeometry& out_geometry) {
    if (!is_busy.load()) return false;

    if (task_future.valid() && task_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        RenderGeometry result = task_future.get();
        is_busy.store(false);
        
        if (should_cancel.load()) return false;

        out_geometry = std::move(result);
        return true;
    }

    return false;
}

void InteractiveViewer::cancel() {
    if (!is_busy.load()) return;
    this->should_cancel.store(true);
    if (this->task_future.valid()) this->task_future.wait();
    this->is_busy.store(false);
}