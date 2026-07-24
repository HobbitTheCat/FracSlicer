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

#ifndef PROJECT_FRAC_SLICER_SHADER_H
#define PROJECT_FRAC_SLICER_SHADER_H

#include <GL/glew.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    void setBool    (const std::string& name, bool value) const;
    void setInt     (const std::string& name, int value) const;
    void setFloat   (const std::string& name, float value) const;
    void setVec2    (const std::string& name, const glm::vec2& value) const;
    void setVec3    (const std::string& name, const glm::vec3& value) const;
    void setMat4    (const std::string& name, const glm::mat4& value) const;

private:
    mutable std::unordered_map<std::string, int> uniformLocationCache;

    int getUniformLocation(const std::string& name) const;
    static std::string readFile(const std::string& filePath);

    static unsigned int compileShader(unsigned int type, const char* source, const std::string& typeName);

    static void checkCompileErrors(unsigned int shader, std::string type);
};

#endif //PROJECT_FRAC_SLICER_SHADER_H