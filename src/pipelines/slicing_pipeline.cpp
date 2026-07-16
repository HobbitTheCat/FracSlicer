#include "pipelines/slicing_pipeline.h"
#include "pipelines/pipe_data.h"

#include "core/fractal_filters.h"
#include "services/fractal_generator.h"
#include "services/layer_slicer.h"
#include "services/rasterization_service.h"

#include <chrono>
#include <cstdio>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

SlicingPipeline::~SlicingPipeline () {
    this->cancel();
}

void SlicingPipeline::start_slicing(
    std::shared_ptr<const FractalModel> model,
    int target_iteration,
    const arma::rowvec& plane_normal,
    double initial_z_offset,
    const Printer& printer, // TODO Check if a printer is needed here, or if I can just retrieve it from the encoder using `get_printer`
    std::shared_ptr<encoder::GooEncoder> encoder
) {
    if (this->is_busy.load()) return;

    this->is_busy.store(true);
    this->progress.store(0.0f);
    this->should_cancel.store(false);

    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        std::queue<LayerPreview>().swap(this->preview_queue);
    }

    this->task_future = std::async(std::launch::async, [this, model, target_iteration, plane_normal, initial_z_offset, printer, encoder](){

        double layer_thickness = printer.mm_to_world_z(printer.get_layer_height());
        double initial_z_offset_mm = printer.world_to_mm(initial_z_offset);
        int layer_id = 0;
        bool has_found_geometry = false;

        auto cancel_pred = [this]() { return this->should_cancel.load(); };

        while (!this->should_cancel.load()) {
            printf("Layer id: %d\n", layer_id);
            double z_offset = initial_z_offset + layer_id * layer_thickness;

            SliceContext context;
            context.layer_id = layer_id;
            context.z_offset = z_offset;
            context.plane_normal = plane_normal;
            printf("Z offset %f\n", z_offset);

            auto culling_filter = FractalFilters::plane_intersection(plane_normal, z_offset, 0);
            
            // Step 1: generate fractal layer
            printf("Step 1: generate fractal layer\n");
            context.layer_meshes = FractalGenerator::evaluate(*model, target_iteration, culling_filter, nullptr, cancel_pred);

            if (this->should_cancel.load()) break;

            // Step 2: Layer slicing
            printf("Step 2: Layer slicing\n");
            LayerSlicer::slice_layer(context, *model);

            if (context.flat_edges.empty()) {
                if (has_found_geometry) { printf("Fractal has ended\n"); break;}  // if fractal has ended
                else {layer_id++; printf("Fractal did not start\n"); continue;}   // if fractal did not start
            } else {
                printf("There is %ld instances\n", context.layer_meshes.size());
                has_found_geometry = true;
            }
            
            // Step 3: Rasterization
            printf("Step 3: Rasterization\n");
            RasterizationService::rasterize_layer(context, printer, {1920, 1080, true});

            // Step 4: Save to .goo
            double layer_thickness_mm = printer.get_layer_height();
            float z_offset_mm = initial_z_offset_mm + layer_id * layer_thickness_mm;
            printf("Step 4: Save to .goo\n");
            encoder->write_layer(z_offset_mm, context.layer_encoder);

            // Step 5: Generation of preview
            printf("Step 5: preview generation\n");
            LayerPreview preview;
            preview.layer_id = layer_id;

            preview.z_offset = z_offset_mm; 
            preview.buffer = std::move(context.preview_buffer);

            {
                std::lock_guard<std::mutex> lock(this->queue_mutex);
                this->preview_queue.push(std::move(preview));
            }

            // progress update
            layer_id ++;
            // if ((layer_id > 1000 && target_iteration == 0) || layer_id > 5000) {printf("ERROR: Slicing pipeline: Too much layers\n"); break;}
        }

        encoder->close();
        this->is_busy.store(false);
    });
}

bool SlicingPipeline::update(std::vector<LayerPreview>& out_previews) {
    bool has_new_data = false;

    {
        std::lock_guard<std::mutex> lock(this->queue_mutex); // TODO check lock acquisition time
        while (!preview_queue.empty()) {
            out_previews.push_back(std::move(this->preview_queue.front()));
            this->preview_queue.pop();
            has_new_data = true;
        }
    }

    if (!this->is_busy.load() &&
        task_future.valid() &&
        task_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) 
    {
        this->task_future.get();
    }

    return has_new_data;
}

void SlicingPipeline::cancel() {
    if (!is_busy.load()) return;
    this->should_cancel.store(true);
    if (this->task_future.valid()) this->task_future.wait();
    this->is_busy.store(false);
}