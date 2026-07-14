#include "renderer/slice_renderer.h"

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <vector>

SliceRenderer::~SliceRenderer() {
    this->clear_layers();
    if (this->quad_vao) glDeleteVertexArrays(1, &this->quad_vao);
    if (this->quad_vbo) glDeleteBuffers(1, &this->quad_vbo);
}

void SliceRenderer::init(const char* vertex_path, const char* fragment_path, int preview_w, int preview_h, float phys_width, float phys_height) {
    this->shader = std::make_unique<Shader>(vertex_path, fragment_path);

    this->texture_width = preview_w;
    this->texture_height = preview_h;
    this->physical_size = glm::vec2(phys_width, phys_height);

    this->init_quad();
}

void SliceRenderer::init_quad() {
    float quad_vertices[] = {
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void SliceRenderer::add_layer(int layer_id, double z_offset, const std::vector<uint8_t>& pixel_data) {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, pixel_data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D, 0);
    this->layers.push_back({layer_id, z_offset, tex_id});
}

void SliceRenderer::clear_layers() {
    for (auto& layer : this->layers) {
        glDeleteTextures(1, &layer.texture_id);
    }
    this->layers.clear();
}

void SliceRenderer::update_physical_size(float phys_width, float phys_height) {
    this->physical_size = glm::vec2(phys_width, phys_height);
}

void SliceRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!this->shader || this->layers.empty()) return;
    
    this->shader->use();
    this->shader->setVec3("sliceColor", glm::vec3(0.0f, 0.8, 1.0f));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(quad_vao);
        
    for (const auto& layer : this->layers) {
        glm::mat4 slice_to_printer = glm::mat4(1.0f);
        slice_to_printer = glm::translate(slice_to_printer, 
                    glm::vec3(this->physical_size.x / 2.0f, this->physical_size.y / 2.0f, static_cast<float>(layer.z_offset)));
        slice_to_printer = glm::scale(slice_to_printer, 
                    glm::vec3(this->physical_size.x, this->physical_size.y, 1.0f));

        glm::mat4 model = this->m_printer_to_world * slice_to_printer;
        
        glm::mat4 mvp = projection * view * model;
        
        this->shader->setMat4("mvp", mvp);

        // shader->setFloat("z_offset", static_cast<float>(layer.z_offset));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, layer.texture_id);
        shader->setInt("sliceTexture", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}
