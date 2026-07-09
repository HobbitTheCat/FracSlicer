#ifndef PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H
#define PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H

#include "core/geometry_types.h"
#include <vector>

struct RenderGeometry {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

namespace GeometryConverter {

    /**
     * @brief Converts MeshTemplate into flat vectors for OpenGL.
     * This operation runs in parallel using std::execution::par to speed up the float conversion.
     * 
     */
    RenderGeometry convert_to_render_geometry(MeshTemplate&& mesh);
    
} // namespace GeometryConverter

#endif //PROJECT_FRAC_SLICER_GEOMETRY_CONVERTER_H