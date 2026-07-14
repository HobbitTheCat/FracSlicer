#ifndef PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H
#define PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H

#include "renderer.h"
#include "shader.h"
#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class FractalRenderer : public IRenderer {
private:
    std::unique_ptr<Shader> shader = nullptr;

    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei index_count = 0;

    GLuint plane_vao = 0, plane_vbo = 0;

    glm::mat4 model_matrix = glm::mat4(1.0f);
    bool draw_plane = false;
    glm::mat4 plane_model_matrix = glm::mat4(1.0f);
public:
    FractalRenderer() = default;
    ~FractalRenderer() override;

    void init(const char* vertex_path, const char* fragment_path);

    void update_geometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

    void set_model_matrix(const glm::mat4& model) {this->model_matrix = model;}
    void set_plane_settings(bool draw, const glm::mat4& model) {
        this->draw_plane = draw;
        this->plane_model_matrix = model;
    }
    
    void render(const glm::mat4& view, const glm::mat4& projection) override;
};

#endif //PROJECT_FRAC_SLICER_FRACTAL_RENDERER_H