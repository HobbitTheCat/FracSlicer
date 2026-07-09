#ifndef PROJECT_FRAC_SLICER_SLICING_PIPELINE_H
#define PROJECT_FRAC_SLICER_SLICING_PIPELINE_H

#include "core/fractal_model.h"
#include "core/printer.h"

#include "encoder/goo_encoder.h"
#include "pipelines/pipe_data.h"

#include <cstdint>
#include <future>
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <vector>

struct LayerPreview {
    int layer_id;
    double z_offset;
    std::vector<uint8_t> buffer;
};

class SlicingPipeline {
private:
    std::future<void> task_future;
    std::atomic<bool> is_busy{false};
    std::atomic<float> progress{0.0f};
    std::atomic<bool> should_cancel{false};

    std::mutex queue_mutex;
    std::queue<std::vector<Edge>> preview_queue;

public:
    SlicingPipeline() = default;
    ~SlicingPipeline();

    void start_slicing(
        std::shared_ptr<const FractalModel> model,
        int target_iteration,
        const arma::rowvec& plane_normal,
        double initial_z_offset,
        const Printer& printer,
        std::shared_ptr<encoder::GooEncoder> encoder // TODO Create an Encoder abstraction layer
    );

    bool update(std::vector<std::vector<Edge>>& out_previews);

    bool is_working() const {return this->is_busy.load();}
    float get_progress() const {return this->progress.load();}
    void cancel();
};

#endif //PROJECT_FRAC_SLICER_SLICING_PIPELINE_H
