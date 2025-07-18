#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;  // 10 m/s base human walking speed
const float SENSITIVITY = 0.1f;  // Simple sensitivity
const float ZOOM = 45.0f;

class Camera {
public:
    glm::dvec3 Position;  // Use double precision for large coordinates
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    float Yaw;
    float Pitch;
    
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    
    Camera(glm::dvec3 position = glm::dvec3(0.0, 0.0, 3.0), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, float pitch = PITCH);
    
    Camera(double posX, double posY, double posZ, 
           float upX, float upY, float upZ, 
           float yaw, float pitch);
    
    glm::mat4 GetViewMatrix();
    
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    
private:
    void updateCameraVectors();
};