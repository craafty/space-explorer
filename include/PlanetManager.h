#pragma once

#include "Planet.h"
#include <epoxy/gl.h>
#include <vector>
#include <memory>

class PlanetManager {
public:
    PlanetManager();
    ~PlanetManager();
    
    void Initialize();
    void Update(const glm::vec3& cameraPos);
    void Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos);
    
    // Planet access
    size_t GetPlanetCount() const { return m_planets.size(); }
    Planet* GetPlanet(size_t index);
    Planet* GetPlanetByName(const std::string& name);
    
    // Navigation helpers
    glm::vec3 GetPlanetPosition(const std::string& name);
    float GetDistanceToPlanet(const std::string& name, const glm::vec3& position);
    
    // Statistics
    int GetTotalTriangles() const;
    int GetTotalActiveNodes() const;
    
private:
    std::vector<std::unique_ptr<Planet>> m_planets;
    
    void CreateSolarSystem();
    PlanetData CreateEarth();
    PlanetData CreateMoon();
    PlanetData CreateMars();
    PlanetData CreateSun();
};