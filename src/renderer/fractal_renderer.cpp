#include "renderer/fractal_renderer.h"
#include "renderer/shader.h"

#include <glm/ext/vector_float3.hpp>
#include <iostream>

FractalRenderer::~FractalRenderer() {
    this->cleanup_framebuffer();
    
    if (this->vao) glDeleteVertexArrays(1, &this->vao);
    if (this->vbo) glDeleteBuffers(1, &this->vbo);
    if (this->ebo) glDeleteBuffers(1, &this->ebo);

    if (this->plane_vao) glDeleteVertexArrays(1, &this->plane_vao);
    if (this->plane_vbo) glDeleteBuffers(1, &this->plane_vbo);
}

void FractalRenderer::init(const char* vertex_path, const char* fragment_path) {
    this->shader = std::make_unique<Shader>(vertex_path, fragment_path);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glGenVertexArrays(1, &this->plane_vao);
    glGenBuffers(1, &this->plane_vbo);

    float plane_vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
            
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    
    glBindVertexArray(this->plane_vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->plane_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void FractalRenderer::update_geometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    if (vertices.empty() || indices.empty()) {
        this->index_count = 0;
        return;
    }
    
    this->index_count = static_cast<GLsizei>(indices.size());

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void FractalRenderer::setup_framebuffer(int width, int height) {
    if (this->fbo && this->current_width == width && this->current_height == height) return;

    this->cleanup_framebuffer();

    this->current_height = height;
    this->current_width = width;

    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    glGenTextures(1, &this->texture_id);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture_id, 0);

    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FractalRenderer::cleanup_framebuffer() {
    if (this->fbo) glDeleteFramebuffers(1, &this->fbo);
    if (this->texture_id) glDeleteTextures(1, &this->texture_id);
    if (this->rbo) glDeleteRenderbuffers(1, &this->rbo);
    
    this->fbo = 0;
    this->texture_id = 0;
    this->rbo = 0;
}

GLuint FractalRenderer::render_to_texture(
    int width, int height,
    const glm::mat4& view_matrix,
    const glm::mat4& projection_matrix,
    bool draw_plane,
    const glm::mat4& plane_model_matrix
) {
    if (width <= 0 || height <= 0) return 0;

    this->setup_framebuffer(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Цвет фона
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->shader) {
        this->shader->use();
        glm::mat4 mvp = projection_matrix * view_matrix;

        if (this->index_count > 0) {
            this->shader->setMat4("mvp", mvp);
            this->shader->setVec3("color", glm::vec3(0.8f, 0.5f, 0.2f));
            this->shader->setFloat("alpha", 1.0f);
    
            glBindVertexArray(this->vao);
            glDrawElements(GL_TRIANGLES, this->index_count, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        if (draw_plane && this->plane_vao) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 plane_mvp = mvp * plane_model_matrix;

            this->shader->setMat4("mvp", plane_mvp);
            this->shader->setVec3("color", glm::vec3(0.2f, 0.6f, 1.0f));
            this->shader->setFloat("alpha", 0.4f);

            glBindVertexArray(this->plane_vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            glDisable(GL_BLEND);
        }
    }
    
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return this->texture_id;
}