#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform float z_offset;
uniform vec2 plane_size; 

out vec2 TexCoord;

void main() {
    vec3 world_pos = vec3(aPos.x * plane_size.x, aPos.y * plane_size.y, z_offset);
    gl_Position = projection * view * vec4(world_pos, 1.0);
    TexCoord = aTexCoord;
}