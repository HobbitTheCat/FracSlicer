#include "application/application.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

void Application::Init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(1280, 720, "Fractal Slicer & Viewer", NULL, NULL);
    if (!this->window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1); // TODO verify

    glfwSetWindowUserPointer(this->window, this);
    glfwSetCursorPosCallback(this->window, Application::mouse_callback);
    glfwSetScrollCallback(this->window, Application::scroll_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("res/font/Roboto-Medium.ttf", 19);
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->fractal_renderer.init("res/shaders/fractal_renderer/vertex.glsl", "res/shaders/fractal_renderer/fragment.glsl");
        
    this->slice_renderer.init("res/shaders/slice_renderer/vertex.glsl", "res/shaders/slice_renderer/fragment.glsl", 1920, 1080, 218.88f, 122.88f);

    this->camera.fov = 45.0f;
    this->camera.cameraDist = 5.0f;
    this->camera.pitch = 0.0f;
    this->camera.yaw = 0.0f;
    this->camera.isDragging = false;
    this->camera.firstMouse = true;

    this->gui = std::make_unique<FractalGui>(
        this->interactive_viewer,
        this->slicing_pipeline,
        this->fractal_renderer,
        this->slice_renderer,
        this->current_model,
        this->camera
    );
}

void Application::Cleanup() {
    this->gui.reset();
    this->interactive_viewer.cancel();
    this->slicing_pipeline.cancel();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (this->window) glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Application::Run() {
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();

        RenderGeometry new_geo;
        if (this->interactive_viewer.update(new_geo)) {
            this->fractal_renderer.update_geometry(new_geo.vertices, new_geo.indices);
        }

        std::vector<LayerPreview> new_previews;
        if (this->slicing_pipeline.update(new_previews)) {
            for (const auto& preview : new_previews) {
                this->slice_renderer.add_layer(preview.layer_id, preview.z_offset, preview.buffer);
            }
        }

        int fb_width, fb_height;
        glfwGetFramebufferSize(this->window, &fb_width, &fb_height);
        this->camera.windowWidth = static_cast<float>(fb_width);
        this->camera.windowHeight = static_cast<float>(fb_height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        this->gui->render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(this->window);
    }
}

void Application::mouse_callback(GLFWwindow *window, double xpos_in, double ypos_in) {
    auto* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    float xpos = static_cast<float>(xpos_in);
    float ypos = static_cast<float>(ypos_in);

    if (app->camera.firstMouse) {
        app->camera.lastX = xpos;
        app->camera.lastY = ypos;
        app->camera.firstMouse = false;
    }

    float xoffset = xpos - app->camera.lastX;
    float yoffset = app->camera.lastY - ypos;
    app->camera.lastX = xpos;
    app->camera.lastY = ypos;

    if (app->camera.isDragging) {
        float sensitivity = 0.1f;
        app->camera.yaw += xoffset * sensitivity;
        app->camera.pitch += yoffset * sensitivity;

        if (app->camera.pitch > 89.0f) app->camera.pitch = 89.0f;
        if (app->camera.pitch < -89.0f) app->camera.pitch = -89.0f;
    }
}

void Application::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    // if (io.WantCaptureMouse) return;

    auto* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    app->camera.cameraDist -= static_cast<float>(yoffset) * 0.05f;
    if (app->camera.cameraDist < 0.5f) app->camera.cameraDist = 0.5f;
    if (app->camera.cameraDist > 100.0f) app->camera.cameraDist = 100.0f;
}