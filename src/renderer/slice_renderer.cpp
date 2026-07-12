#include "renderer/slice_renderer.h"

#include <cstdint>
#include <vector>

SliceRenderer::~SliceRenderer() {
    this->clear_layers();
    this->cleanup_framebuffer();
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

GLuint SliceRenderer::render_to_texture(int width, int height, const glm::mat4& view_matrix, const glm::mat4& projection_matrix) {
    if (width != current_width || height != current_height) {
        setup_framebuffer(width, height);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    this->shader->use();

    this->shader->setMat4("view", view_matrix);
    this->shader->setMat4("projection", projection_matrix);
    this->shader->setVec3("sliceColor", glm::vec3(0.0f, 0.8, 1.0f));
    this->shader->setVec2("plane_size", this->physical_size);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    glBindVertexArray(quad_vao);
    for (const auto& layer : this->layers) {
        shader->setFloat("z_offset", static_cast<float>(layer.z_offset));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, layer.texture_id);
        shader->setInt("sliceTexture", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return scene_texture_id;
}

void SliceRenderer::setup_framebuffer(int width, int height) {
    this->cleanup_framebuffer();
    
    current_width = width;
    current_height = height;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &scene_texture_id);
    glBindTexture(GL_TEXTURE_2D, scene_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_texture_id, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // FBO ERROR
        printf("FBO ERROR\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SliceRenderer::cleanup_framebuffer() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (scene_texture_id) glDeleteTextures(1, &scene_texture_id);
    if (rbo) glDeleteRenderbuffers(1, &rbo);
}

void SliceRenderer::update_physical_size(float phys_width, float phys_height) {
    this->physical_size = glm::vec2(phys_width, phys_height);
}