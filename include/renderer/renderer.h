#ifndef PROJECT_FRAC_SLICER_I_RENDERER_H
#define PROJECT_FRAC_SLICER_I_RENDERER_H

#include <glm/fwd.hpp>

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void render(const glm::mat4& view, const glm::mat4& projection) = 0;
};

#endif //PROJECT_FRAC_SLICER_I_RENDERER_H