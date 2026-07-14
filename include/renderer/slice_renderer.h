#ifndef PROJECT_FRAC_SLICER_SLICE_RENDERER_H
#define PROJECT_FRAC_SLICER_SLICE_RENDERER_H

#include "renderer/renderer.h"
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

class SliceRenderer : public IRenderer{
private:
    std::unique_ptr<Shader> shader = nullptr;
    GLuint quad_vao = 0, quad_vbo = 0;

    std::vector<SliceLayerGL> layers;

    int texture_width = 0;
    int texture_height = 0;
    glm::vec2 physical_size;

    glm::mat4 m_printer_to_world = glm::mat4(1.0f);

    void init_quad();
    
public:
    SliceRenderer() = default;
    ~SliceRenderer() override;

    void init(const char* vertex_path, const char* fragment_path, int preview_w, int preview_h, float phys_width, float phys_height);

    void add_layer(int layer_id, double z_offset, const std::vector<uint8_t>& pixel_data);
    void clear_layers();

    void update_physical_size(float phys_width, float phys_height);

    void set_printer_to_world_matrix(const glm::mat4& mat) {this->m_printer_to_world = mat;}
    
    void render(const glm::mat4& view, const glm::mat4& projection) override;
};

#endif //PROJECT_FRAC_SLICER_SLICE_RENDERER_H