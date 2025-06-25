#pragma once

#include "planet.h"
#include <vector>
#include <memory>

class PlanetManager {
public:
    PlanetManager();
    ~PlanetManager();
    
    void Update(const glm::vec3& cameraPos, float deltaTime);
    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos);
    
    const std::vector<std::unique_ptr<Planet>>& GetPlanets() const { return m_planets; }
    
    int GetTotalTriangleCount() const;
    int GetTotalNodeCount() const;
    
private:
    std::vector<std::unique_ptr<Planet>> m_planets;
    
    void CreatePlanets();
};