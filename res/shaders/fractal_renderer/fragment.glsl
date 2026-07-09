#version 330 core
out vec4 FragColor;
in vec3 FragPos;

uniform vec3 color;
uniform float alpha = 1.0;

void main() {
    vec3 dx = dFdx(FragPos);
    vec3 dy = dFdy(FragPos);
    
    vec3 normal = normalize(cross(dx, dy));
    
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.8));
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 ambient = 0.3 * color;
    vec3 diffuse = diff * color;
    
    FragColor = vec4(ambient + diffuse, alpha);
}