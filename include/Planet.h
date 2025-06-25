#pragma once

#include "CubeSphere.h"
#include <glm/glm.hpp>
#include <epoxy/gl.h>
#include <string>
#include <memory>

struct PlanetData {
    std::string name;
    float radius;           // in meters (real life-size scale)
    glm::vec3 position;     // position in space (real scale in meters)
    glm::vec3 color;        // planet color
    float rotationSpeed;    // rotation speed
    
    // No scaling - everything is life-size!
    // Human height ~ 1.7 meters for reference
};

class Planet {
public:
    Planet(const PlanetData& data);
    ~Planet();
    
    void Update(const glm::vec3& cameraPos, float deltaTime);
    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos);
    
    const PlanetData& GetData() const { return m_data; }
    glm::vec3 GetPosition() const { return m_data.position; }
    float GetRadius() const { return m_data.radius; }  // Already in meters
    float GetRadiusKm() const { return m_data.radius / 1000.0f; }
    
    int GetTriangleCount() const;
    int GetActiveNodeCount() const;
    
private:
    PlanetData m_data;
    std::unique_ptr<CubeSphere> m_sphere;
    float m_currentRotation;
};