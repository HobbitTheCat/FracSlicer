#ifndef PROJECT_FRAC_SLICER_SCENE_H
#define PROJECT_FRAC_SLICER_SCENE_H

#include "renderer.h"
#include <GL/glew.h>
#include <vector>

class Scene {
private:
    std::vector<IRenderer*> renderers;

    GLuint fbo = 0;
    GLuint color_texture = 0;
    GLuint rbo_depth = 0;
    
    int current_width = 0;
    int current_height = 0;

    void setup_framebuffer(int width, int height);

public:
    Scene() = default;
    ~Scene() { this->cleanup_framebuffer(); }

    void cleanup_framebuffer();

    void add_renderer(IRenderer* renderer);
    void clear_renderers();

    GLuint render_scene(int width, int height, const glm::mat4& view, const glm::mat4& projection);
};

#endif //PROJECT_FRAC_SLICER_SCENE_H