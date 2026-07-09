#include "renderer/edge_renderer.h"
#include <iostream>

EdgeRenderer::~EdgeRenderer() {
    this->cleanup_framebuffer();
    if (this->vao) glDeleteVertexArrays(1, &this->vao);
    if (this->vbo) glDeleteBuffers(1, &this->vbo);
}

void EdgeRenderer::init(const char* vertex_path, const char* fragment_path) {
    this->shader = std::make_unique<Shader>(vertex_path, fragment_path);

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
}

void EdgeRenderer::add_edges(const std::vector<Edge>& edges) {
    if (edges.empty()) return;

    this->accumulated_vertices.reserve(this->accumulated_vertices.size() + edges.size() * 2 * 3);

    for (const auto& edge : edges) {
        this->accumulated_vertices.push_back(static_cast<float>(edge.p0(0)));
        this->accumulated_vertices.push_back(static_cast<float>(edge.p0(1)));
        this->accumulated_vertices.push_back(static_cast<float>(edge.z_offset));

        this->accumulated_vertices.push_back(static_cast<float>(edge.p1(0)));
        this->accumulated_vertices.push_back(static_cast<float>(edge.p1(1)));
        this->accumulated_vertices.push_back(static_cast<float>(edge.z_offset));
    }

    this->vertex_count = static_cast<GLsizei>(this->accumulated_vertices.size() / 3);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    
    glBufferData(GL_ARRAY_BUFFER, 
                 this->accumulated_vertices.size() * sizeof(float), 
                 this->accumulated_vertices.data(), 
                 GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void EdgeRenderer::clear_edges() {
    this->accumulated_vertices.clear();
    this->vertex_count = 0;
    
    // Опционально: можно занулить буфер в GPU, чтобы не держать старую память
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
}

void EdgeRenderer::setup_framebuffer(int width, int height) {
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

void EdgeRenderer::cleanup_framebuffer() {
    if (this->fbo) glDeleteFramebuffers(1, &this->fbo);
    if (this->texture_id) glDeleteTextures(1, &this->texture_id);
    if (this->rbo) glDeleteRenderbuffers(1, &this->rbo);

    this->fbo = 0;
    this->texture_id = 0;
    this->rbo = 0;
}

GLuint EdgeRenderer::render_to_texture(int width, int height, const glm::mat4& view_matrix, const glm::mat4& projection_matrix) {
    if (width <= 0 || height <= 0) return 0;

    this->setup_framebuffer(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->shader && this->vertex_count > 0) {
        this->shader->use();

        glm::mat4 mvp = projection_matrix * view_matrix;

        this->shader->setMat4("mvp", mvp);
        this->shader->setVec3("color", glm::vec3(0.0f, 1.0f, 0.5f));
        this->shader->setFloat("alpha", 1.0f);

        glBindVertexArray(this->vao);
        glDrawArrays(GL_LINES, 0, this->vertex_count); 
        glBindVertexArray(0);
        
    }

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return this->texture_id;
}