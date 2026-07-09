#ifndef PROJECT_FRAC_SLICER_APPLICATION_H
#define PROJECT_FRAC_SLICER_APPLICATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/fractal_model.h"
#include "pipelines/interactive_view.h"
#include "pipelines/slicing_pipeline.h"
#include "renderer/edge_renderer.h"
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
    EdgeRenderer edge_renderer;

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