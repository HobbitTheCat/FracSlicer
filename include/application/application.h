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

#ifndef PROJECT_FRAC_SLICER_APPLICATION_H
#define PROJECT_FRAC_SLICER_APPLICATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/fractal_model.h"
#include "pipelines/interactive_view.h"
#include "pipelines/slicing_pipeline.h"
#include "renderer/fractal_renderer.h"
#include "renderer/slice_renderer.h"
#include "fractal_gui.h"

#include <memory>

class Application {
private:
    GLFWwindow* window;
    CameraControls camera;

    std::shared_ptr<const FractalModel> current_model;

    InteractiveViewer interactive_viewer;
    SlicingPipeline slicing_pipeline;

    FractalRenderer fractal_renderer;
    SliceRenderer slice_renderer;

    Scene scene;

    std::unique_ptr<FractalGui> gui;

public:
    Application() = default;
    void Init();
    void Run();
    void Cleanup();

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif //PROJECT_FRAC_SLICER_APPLICATION_H