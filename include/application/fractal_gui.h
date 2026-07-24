/*
 * Copyright (C) 2026 Basilisk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PROJECT_FRAC_SLICER_FRACTAL_GUI_H
#define PROJECT_FRAC_SLICER_FRACTAL_GUI_H

#include "core/fractal_model.h"
#include "renderer/ui_structures.h"
#include "pipelines/interactive_view.h"
#include "pipelines/slicing_pipeline.h"
#include "renderer/fractal_renderer.h"
#include "renderer/slice_renderer.h"
#include "renderer/scene.h"

#include <memory>
#include <string>

class FractalGui {
private:
    InteractiveViewer& viewer;
    SlicingPipeline& slicer;
    FractalRenderer& frac_renderer;
    SliceRenderer& slice_renderer;
    Scene& scene;

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
        SliceRenderer& sr,
        Scene& scn,
        std::shared_ptr<const FractalModel>& model,
        CameraControls& cam
    ) : viewer(v), slicer(s), frac_renderer(fr), slice_renderer(sr), scene(scn), current_model(model), camera(cam) {}

    void render();
};

#endif //PROJECT_FRAC_SLICER_FRACTAL_GUI_H