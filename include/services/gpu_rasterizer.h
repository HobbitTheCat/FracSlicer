#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer/shader.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <array>

struct RasterizationResult {
    std::vector<uint8_t> printer_pixels;
    std::vector<uint8_t> preview_pixels;
};

class GpuRasterizer {
private:
    GLuint fbo_full, tex_full;
    GLuint fbo_prev, tex_prev;
    GLuint vao, vbo, ebo;

    std::unique_ptr<Shader> shader;

    int full_w, full_h;
    int prev_w, prev_h;

public:
    GpuRasterizer(int f_width, int f_height, int p_width, int p_height);
    ~GpuRasterizer();

    RasterizationResult rasterize_and_read(
        const std::vector<std::array<double, 2>>& vertices,
        const std::vector<uint32_t>& indices,
        const glm::mat4& projection
    );
};