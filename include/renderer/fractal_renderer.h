#ifndef PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H
#define PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H

#include "shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class FractalRenderer {
private:
    std::unique_ptr<Shader> shader = nullptr;

    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei index_count = 0;

    GLuint plane_vao = 0, plane_vbo = 0;

    GLuint fbo = 0;
    GLuint texture_id = 0;
    GLuint rbo = 0;
    int current_width = 0;
    int current_height = 0;

    void setup_framebuffer(int width, int height);
    void cleanup_framebuffer();

public:
    FractalRenderer() = default;
    ~FractalRenderer();

    void init(const char* vertex_path, const char* fragment_path);

    void update_geometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    GLuint render_to_texture(
        int width, int height,
        const glm::mat4& view_matrix,
        const glm::mat4& projection_matrix,
        bool draw_plane,
        const glm::mat4& plane_model_matrix
    );
};

#endif //PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H