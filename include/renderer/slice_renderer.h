#ifndef PROJECT_FRAC_SLICER_SLICE_RENDERER_H
#define PROJECT_FRAC_SLICER_SLICE_RENDERER_H

#include "shader.h"

#include <GL/glew.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>
#include <cstdint>

// TODO rewrite using GL_TEXTURE_2D_ARRAY for single instruction stack output

struct SliceLayerGL {
    int layer_id;
    double z_offset;
    GLuint texture_id;
};

class SliceRenderer {
private:
    std::unique_ptr<Shader> shader = nullptr;
    GLuint quad_vao = 0, quad_vbo = 0;

    std::vector<SliceLayerGL> layers;

    int texture_width = 0;
    int texture_height = 0;
    glm::vec2 physical_size;

    GLuint fbo = 0;
    GLuint rbo = 0;
    GLuint scene_texture_id = 0;
    int current_width = 0;
    int current_height = 0;

    void setup_framebuffer(int width, int height);
    void cleanup_framebuffer();
    void init_quad();
    
public:
    SliceRenderer() = default;
    ~SliceRenderer();

    void init(const char* vertex_path, const char* fragment_path, int preview_w, int preview_h, float phys_width, float phys_height);

    void add_layer(int layer_id, double z_offset, const std::vector<uint8_t>& pixel_data);
    void clear_layers();

    GLuint render_to_texture(int width, int height, const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

    void update_physical_size(float phys_width, float phys_height);
};

#endif //PROJECT_FRAC_SLICER_SLICE_RENDERER_H