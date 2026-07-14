#include "renderer/scene.h"
#include "renderer/renderer.h"

#include <cstdio>
#include <glm/fwd.hpp>
#include <iostream>

void Scene::setup_framebuffer(int width, int height) {
    printf("Resizing FBO\n");
    this->cleanup_framebuffer();
    current_height = height;
    current_width = width;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &this->color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Scene FBO ERROR" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::cleanup_framebuffer() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (color_texture) glDeleteTextures(1, &color_texture);
    if (rbo_depth) glDeleteRenderbuffers(1, &rbo_depth);
    fbo = color_texture = rbo_depth = 0;
    current_width = current_height = 0;
}

void Scene::add_renderer(IRenderer* renderer) {
    if (renderer) this->renderers.push_back(renderer);
}

void Scene::clear_renderers() {
    renderers.clear();
}

GLuint Scene::render_scene(int width, int height, const glm::mat4& view, const glm::mat4& projection) {
    if (width <= 0 || height <= 0) return 0;

    if (fbo == 0) {
        printf("Fbo == 0\n");
        this->setup_framebuffer(width, height);
    } else if (width != current_width) {
        printf("Width: %d != current_width %d\n", width, current_width);
        this->setup_framebuffer(width, height);
    } else if (height != current_height) {
        printf("Height: %d != current_height %d\n", height, current_height);
        this->setup_framebuffer(width, height);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* renderer : this->renderers) renderer->render(view, projection);

    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return this->color_texture;
}