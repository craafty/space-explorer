#pragma once

#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <memory>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

enum class CubeFace {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

class QuadNode {
public:
    QuadNode(glm::vec2 topLeft, glm::vec2 bottomRight, int level, CubeFace face);
    ~QuadNode();
    
    void Subdivide();
    void Update(const glm::vec3& cameraPos, float radius, int maxLevel);
    void GenerateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius);
    void CountNodes(int& nodeCount) const;
    
    bool ShouldSubdivide(const glm::vec3& cameraPos, float radius, int maxLevel) const;
    glm::vec3 CubeToSphere(const glm::vec3& cubePoint) const;
    
private:
    glm::vec2 m_topLeft, m_bottomRight;
    int m_level;
    CubeFace m_face;
    std::array<std::unique_ptr<QuadNode>, 4> m_children;
    bool m_subdivided;
    
    static constexpr int RESOLUTION = 8;  // Reduced for better performance
    static constexpr float SUBDIVISION_THRESHOLD = 0.1f;  // More aggressive for 25-unit Earth
    
    glm::vec3 GetCubePosition(float u, float v) const;
    void GenerateQuadMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius);
};

class CubeSphere {
public:
    CubeSphere(float radius = 1.0f, int maxLevel = 8);
    ~CubeSphere();
    
    void Update(const glm::vec3& cameraPos);
    void Render();
    
    void SetRadius(float radius) { m_radius = radius; }
    float GetRadius() const { return m_radius; }
    
    int GetTriangleCount() const;
    int GetActiveNodeCount() const;
    
private:
    float m_radius;
    int m_maxLevel;
    std::array<std::unique_ptr<QuadNode>, 6> m_faces;
    
    unsigned int m_VAO, m_VBO, m_EBO;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    
    void InitializeGL();
    void UpdateMesh();
};