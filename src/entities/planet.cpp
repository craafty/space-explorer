#include "planet.h"
#include <epoxy/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Planet::Planet(const PlanetData& data) : m_data(data), m_currentRotation(0.0f) {
    // Create cube sphere with appropriate LOD for 25-unit Earth
    m_sphere = std::make_unique<CubeSphere>(m_data.radius, 6); // Max LOD 6 for better performance
}

Planet::~Planet() = default;

void Planet::Update(const glm::vec3& cameraPos, float deltaTime) {
    // Update rotation based on time (degrees per second)
    m_currentRotation += m_data.rotationSpeed * deltaTime;
    if (m_currentRotation > 360.0f) {
        m_currentRotation -= 360.0f;
    }
    
    // Transform camera position to planet's local coordinate system
    // This accounts for the planet's rotation so LOD stays fixed relative to camera
    glm::vec3 relativePos = cameraPos - m_data.position;
    
    // Apply inverse rotation to get camera position in planet's local space
    glm::mat4 inverseRotation = glm::rotate(glm::mat4(1.0f), 
                                          glm::radians(-m_currentRotation), 
                                          glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec4 localCameraPos = inverseRotation * glm::vec4(relativePos, 1.0f);
    
    m_sphere->Update(glm::vec3(localCameraPos));
}

void Planet::Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) {
    // Create model matrix with position and rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_data.position);
    model = glm::rotate(model, glm::radians(m_currentRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Set planet-specific uniforms (lighting is set globally in main)
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(colorLoc, 0.3f, 0.6f, 1.0f); // Force blue color
    
    m_sphere->Render();
}

int Planet::GetTriangleCount() const {
    return m_sphere->GetTriangleCount();
}

int Planet::GetActiveNodeCount() const {
    return m_sphere->GetActiveNodeCount();
}