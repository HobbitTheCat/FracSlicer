#include "io/stl_export.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace utils {

    Vec3 get_vertex(const std::vector<float> &vertices, unsigned int index) {
        return {
            vertices[index * 3 + 0],
            vertices[index * 3 + 1],
            vertices[index * 3 + 2]
        };
    }

    // TODO rewrite to another algorithm
    Vec3 calculate_normal(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
        Vec3 e1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
        Vec3 e2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

        Vec3 n = {
            e1.y * e2.z - e1.z * e2.y,
            e1.z * e2.x - e1.x * e2.z,
            e1.x * e2.y - e1.y * e2.x
        };

        float length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (length > 0.000001f) {
            n.x /= length; n.y /= length; n.z /= length;
        }
        return n;
    }

    void write_vec3(std::ofstream& file, const Vec3& v) {
        file.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
        file.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        file.write(reinterpret_cast<const char*>(&v.z), sizeof(float));
    }

    void stl_export(const RenderGeometry &geometry, const std::string &file_path) {
        if (geometry.indices.size() % 3 != 0) throw std::runtime_error("Indices array size is not a multiple of 3");
        printf("Saving\n");
        std::ofstream file(file_path, std::ios::binary);
        if (!file.is_open()) throw std::runtime_error("Cannot open file for STL export: " + file_path);

        char header[80] = {0};
        std::string msg = "Binary STL generated from FracSlicer";
        std::memcpy(header, msg.c_str(), msg.size());
        file.write(header, 80);

        uint32_t num_triangles = static_cast<uint32_t>(geometry.indices.size() / 3);
        file.write(reinterpret_cast<const char*>(&num_triangles), sizeof(uint32_t));

        for (size_t i = 0; i < geometry.indices.size(); i += 3) {
            Vec3 v0 = get_vertex(geometry.vertices, geometry.indices[i]);
            Vec3 v1 = get_vertex(geometry.vertices, geometry.indices[i+1]);
            Vec3 v2 = get_vertex(geometry.vertices, geometry.indices[i+2]);
    
            Vec3 normal = calculate_normal(v0, v1, v2);
    
            write_vec3(file, normal);
            write_vec3(file, v0);
            write_vec3(file, v1);
            write_vec3(file, v2);
    
            uint16_t attribute = 0;
            file.write(reinterpret_cast<const char*>(&attribute), sizeof(uint16_t));
        }
        printf("Saved\n");
    }
} //namespace utils