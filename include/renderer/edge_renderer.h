#ifndef PROJECT_FRAC_SLICER_EDGE_RENDERER_H
#define PROJECT_FRAC_SLICER_EDGE_RENDERER_H


#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK

#include "pipelines/pipe_data.h"

#include "shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <armadillo>


class EdgeRenderer {
private:
    std::unique_ptr<Shader> shader = nullptr;

    GLuint vao = 0, vbo = 0;
    GLsizei vertex_count = 0;

    std::vector<float> accumulated_vertices;

    GLuint fbo = 0;
    GLuint texture_id = 0;
    GLuint rbo = 0;
    int current_width = 0;
    int current_height = 0;

    void setup_framebuffer(int width, int height);
    void cleanup_framebuffer();

public:
    EdgeRenderer() = default;
    ~EdgeRenderer();

    void init(const char* vertex_path, const char* fragment_path);

    void add_edges(const std::vector<Edge>& edges);
    void clear_edges();

    GLuint render_to_texture(int width, int height, const glm::mat4& view_matrix, const glm::mat4& projection_matrix);
};


#endif //PROJECT_FRAC_SLICER_EDGE_RENDERER_H