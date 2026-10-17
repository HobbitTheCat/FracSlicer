#include "services/geometry_converter.h"
#include <fstream>
#include <vector>
namespace utils {

    struct Vec3 {float x, y, z; };

    Vec3 get_vertex(const std::vector<float>& vertices, unsigned int index);

    Vec3 calculate_normal(const Vec3& v0, const Vec3& v1, const Vec3& v2);

    void write_vec3(std::ofstream& file, const Vec3& v);

    void stl_export(const RenderGeometry& geometry, const std::string& file_path);
    
} // namespace utils