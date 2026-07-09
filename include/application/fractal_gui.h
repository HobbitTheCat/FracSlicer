#ifndef PROJECT_FRAC_SLICER_FRACTAL_GUI_H
#define PROJECT_FRAC_SLICER_FRACTAL_GUI_H

#include "core/fractal_model.h"
#include "renderer/edge_renderer.h"
#include "renderer/ui_structures.h"
#include "pipelines/interactive_view.h"
#include "pipelines/slicing_pipeline.h"
#include "renderer/fractal_renderer.h"
#include "renderer/slice_renderer.h"

#include <memory>
#include <string>

class FractalGui {
private:
    InteractiveViewer& viewer;
    SlicingPipeline& slicer;
    FractalRenderer& frac_renderer;
    EdgeRenderer& edge_renderer;
    SliceRenderer& slice_renderer;

    std::shared_ptr<const FractalModel>& current_model;
    CameraControls& camera;

    int iteration = 0;
    PlaneSettings plane_settings;

    bool show_plane = false;

    char config_path_buf[512];
    char output_path_buf[512];
    float scale_1_to_mm = 1.0f;
    std::string slicer_error_msg = "";

    void draw_interactive_tab();
    void draw_slicer_tab();
    void draw_viewport();

    bool interactive_tab_open = true;
public:
    FractalGui(
        InteractiveViewer& v, 
        SlicingPipeline& s,
        FractalRenderer& fr, 
        EdgeRenderer& er,
        SliceRenderer& sr,
        std::shared_ptr<const FractalModel>& model,
        CameraControls& cam
    ) : viewer(v), slicer(s), frac_renderer(fr), edge_renderer(er), slice_renderer(sr), current_model(model), camera(cam) {}

    void render();
};

#endif //PROJECT_FRAC_SLICER_FRACTAL_GUI_H