#include "services/gpu_rasterizer.h"
#include <cstdint>
#include <cstdio>
#include <vector>

GpuRasterizer::GpuRasterizer(int f_width, int f_height, int p_width, int p_height) : full_w(f_width), full_h(f_height), prev_w(p_width), prev_h(p_height) {
    glGenFramebuffers(1, &fbo_full);
    glGenTextures(1, &tex_full);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_full);
    
    glBindTexture(GL_TEXTURE_2D, tex_full);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, full_w, full_h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_full, 0);

    glGenFramebuffers(1, &fbo_prev);
    glGenTextures(1, &tex_prev);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_prev);
    
    glBindTexture(GL_TEXTURE_2D, tex_prev);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, prev_w, prev_h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_prev, 0);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    shader = std::make_unique<Shader>("res/shaders/gpu_rasterizer/vertex.glsl", "res/shaders/gpu_rasterizer/fragment.glsl");
}

GpuRasterizer::~GpuRasterizer() {
    glDeleteFramebuffers(1, &fbo_full);
    glDeleteTextures(1, &tex_full);
    glDeleteFramebuffers(1, &fbo_prev);
    glDeleteTextures(1, &tex_prev);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

RasterizationResult GpuRasterizer::rasterize_and_read(
    const std::vector<std::array<double, 2>>& vertices,
    const std::vector<uint32_t>& indices,
    const glm::mat4& projection
) {
    RasterizationResult result;
    result.printer_pixels.resize(full_w * full_h, 0x00);
    result.preview_pixels.resize(prev_w * prev_h, 0x00);
    
    if (indices.empty()) {printf("Empty\n"); return result;}

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(std::array<double, 2>), vertices.data(), GL_DYNAMIC_DRAW);
            
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(std::array<double, 2>), (void*)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_full);
    glViewport(0, 0, full_w, full_h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    shader->setMat4("projection", projection);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_full);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_prev);
    glBlitFramebuffer(
        0, 0, full_w, full_h, 
        0, 0, prev_w, prev_h, 
        GL_COLOR_BUFFER_BIT, 
        GL_NEAREST 
    );

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_full);
    glReadPixels(0, 0, full_w, full_h, GL_RED, GL_UNSIGNED_BYTE, result.printer_pixels.data());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_prev);
    glReadPixels(0, 0, prev_w, prev_h, GL_RED, GL_UNSIGNED_BYTE, result.preview_pixels.data());
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return result;
}