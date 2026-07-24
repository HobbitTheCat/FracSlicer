/*
 * Copyright (C) 2026 Basilisk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PROJECT_FRAC_SLICER_SCENE_H
#define PROJECT_FRAC_SLICER_SCENE_H

#include "renderer.h"
#include <GL/glew.h>
#include <vector>

class Scene {
private:
    std::vector<IRenderer*> renderers;

    GLuint fbo = 0;
    GLuint color_texture = 0;
    GLuint rbo_depth = 0;
    
    int current_width = 0;
    int current_height = 0;

    void setup_framebuffer(int width, int height);

public:
    Scene() = default;
    ~Scene() { this->cleanup_framebuffer(); }

    void cleanup_framebuffer();

    void add_renderer(IRenderer* renderer);
    void clear_renderers();

    GLuint render_scene(int width, int height, const glm::mat4& view, const glm::mat4& projection);
};

#endif //PROJECT_FRAC_SLICER_SCENE_H