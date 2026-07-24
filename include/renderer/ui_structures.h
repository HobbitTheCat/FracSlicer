#pragma once

#include <glm/vec3.hpp>

struct PlaneSettings {
    bool enabled = false;
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
    float z_offset = 0.0f;
    float size = 5.0f;
    glm::vec3 color = glm::vec3(0.2f, 0.6f, 1.0f);
    int cut_mode = 0;
};

struct CameraControls {
    float windowWidth = 1280.0f;
    float windowHeight = 720.0f;
    float lastX = 640.0f;
    float lastY = 360.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float fov = 45.0f;
    float cameraDist = 5.0f;
    
    bool firstMouse = true;
    bool isDragging = false;
};