#include "planetManager.h"

PlanetManager::PlanetManager() {
    CreatePlanets();
}

PlanetManager::~PlanetManager() = default;

void PlanetManager::CreatePlanets() {
    // Sun
    PlanetData sunData = {
        "Sun",
        696340000.0f,           // 696,340 km radius
        glm::vec3(0.0f),        // At origin
        glm::vec3(1.0f, 1.0f, 0.3f),
        0.0f                    // No rotation for now
    };
    m_planets.push_back(std::make_unique<Planet>(sunData));
    
    // Earth
    PlanetData earthData = {
        "Earth", 
        6371000.0f,             // 6,371 km radius
        glm::vec3(149597870700.0f, 0.0f, 0.0f),  // 1 AU from Sun
        glm::vec3(0.3f, 0.6f, 1.0f),
        0.01f                   // Slow rotation
    };
    m_planets.push_back(std::make_unique<Planet>(earthData));
    
    // Moon
    PlanetData moonData = {
        "Moon",
        1737100.0f,             // 1,737 km radius
        glm::vec3(149597870700.0f + 384400000.0f, 0.0f, 0.0f), // 384,400 km from Earth
        glm::vec3(0.7f, 0.7f, 0.7f),
        0.005f
    };
    m_planets.push_back(std::make_unique<Planet>(moonData));
    
    // Mars
    PlanetData marsData = {
        "Mars",
        3389000.0f,             // 3,389 km radius  
        glm::vec3(227900000000.0f, 0.0f, 0.0f), // 227.9 million km from Sun
        glm::vec3(0.8f, 0.4f, 0.2f),
        0.008f
    };
    m_planets.push_back(std::make_unique<Planet>(marsData));
}

void PlanetManager::Update(const glm::vec3& cameraPos, float deltaTime) {
    for (auto& planet : m_planets) {
        planet->Update(cameraPos, deltaTime);
    }
}

void PlanetManager::Render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) {
    for (auto& planet : m_planets) {
        planet->Render(shaderProgram, view, projection, viewPos);
    }
}

int PlanetManager::GetTotalTriangleCount() const {
    int total = 0;
    for (const auto& planet : m_planets) {
        total += planet->GetTriangleCount();
    }
    return total;
}

int PlanetManager::GetTotalNodeCount() const {
    int total = 0;
    for (const auto& planet : m_planets) {
        total += planet->GetActiveNodeCount();
    }
    return total;
}