#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D sliceTexture;
uniform vec3 sliceColor;

void main() {
    float mask = texture(sliceTexture, TexCoord).r;
    
    if (mask < 0.1) {
        discard; 
    }
    
    FragColor = vec4(sliceColor, 0.4);
}