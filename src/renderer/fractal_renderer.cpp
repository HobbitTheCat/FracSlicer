#include "renderer/fractal_renderer.h"
#include "renderer/shader.h"

#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <iostream>

FractalRenderer::~FractalRenderer() {
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

void FractalRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!this->shader) return;
    
    this->shader->use();

    if (this->index_count > 0) {
        glm::mat4 mvp = projection * view * this->model_matrix;

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

        glm::mat4 plane_mvp = projection * view * this->model_matrix * this->plane_model_matrix;

        this->shader->setMat4("mvp", plane_mvp);
        this->shader->setVec3("color", glm::vec3(0.2f, 0.6f, 1.0f));
        this->shader->setFloat("alpha", 0.4f);

        glBindVertexArray(this->plane_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
    }
}