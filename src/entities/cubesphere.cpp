#include "cubesphere.h"
#include <epoxy/gl.h>
#include <cmath>
#include <iostream>

QuadNode::QuadNode(glm::vec2 topLeft, glm::vec2 bottomRight, int level, CubeFace face)
    : m_topLeft(topLeft), m_bottomRight(bottomRight), m_level(level), m_face(face), m_subdivided(false) {
}

QuadNode::~QuadNode() = default;

void QuadNode::Subdivide() {
    if (m_subdivided) return;
    
    glm::vec2 center = (m_topLeft + m_bottomRight) * 0.5f;
    
    m_children[0] = std::make_unique<QuadNode>(m_topLeft, center, m_level + 1, m_face);
    m_children[1] = std::make_unique<QuadNode>(glm::vec2(center.x, m_topLeft.y), glm::vec2(m_bottomRight.x, center.y), m_level + 1, m_face);
    m_children[2] = std::make_unique<QuadNode>(glm::vec2(m_topLeft.x, center.y), glm::vec2(center.x, m_bottomRight.y), m_level + 1, m_face);
    m_children[3] = std::make_unique<QuadNode>(center, m_bottomRight, m_level + 1, m_face);
    
    m_subdivided = true;
}

void QuadNode::Update(const glm::vec3& cameraPos, float radius, int maxLevel) {
    if (ShouldSubdivide(cameraPos, radius, maxLevel)) {
        if (!m_subdivided) {
            Subdivide();
        }
        for (auto& child : m_children) {
            if (child) {
                child->Update(cameraPos, radius, maxLevel);
            }
        }
    } else {
        m_subdivided = false;
        for (auto& child : m_children) {
            child.reset();
        }
    }
}

bool QuadNode::ShouldSubdivide(const glm::vec3& cameraPos, float radius, int maxLevel) const {
    if (m_level >= maxLevel) return false;
    
    glm::vec2 center = (m_topLeft + m_bottomRight) * 0.5f;
    glm::vec3 cubeCenter = GetCubePosition(center.x, center.y);
    glm::vec3 sphereCenter = CubeToSphere(cubeCenter) * radius;
    
    float distance = glm::length(cameraPos - sphereCenter);
    float nodeSize = glm::length(m_bottomRight - m_topLeft) * radius;
    
    return (nodeSize / distance) > SUBDIVISION_THRESHOLD;
}

glm::vec3 QuadNode::GetCubePosition(float u, float v) const {
    switch (m_face) {
        case CubeFace::FRONT:  return glm::vec3(u * 2.0f - 1.0f, v * 2.0f - 1.0f, 1.0f);
        case CubeFace::BACK:   return glm::vec3(1.0f - u * 2.0f, v * 2.0f - 1.0f, -1.0f);
        case CubeFace::LEFT:   return glm::vec3(-1.0f, v * 2.0f - 1.0f, 1.0f - u * 2.0f);
        case CubeFace::RIGHT:  return glm::vec3(1.0f, v * 2.0f - 1.0f, u * 2.0f - 1.0f);
        case CubeFace::TOP:    return glm::vec3(u * 2.0f - 1.0f, 1.0f, 1.0f - v * 2.0f);
        case CubeFace::BOTTOM: return glm::vec3(u * 2.0f - 1.0f, -1.0f, v * 2.0f - 1.0f);
        default: return glm::vec3(0.0f);
    }
}

glm::vec3 QuadNode::CubeToSphere(const glm::vec3& cubePoint) const {
    return glm::normalize(cubePoint);
}

void QuadNode::GenerateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius) {
    if (m_subdivided) {
        for (auto& child : m_children) {
            if (child) {
                child->GenerateMesh(vertices, indices, radius);
            }
        }
    } else {
        GenerateQuadMesh(vertices, indices, radius);
    }
}

void QuadNode::GenerateQuadMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius) {
    unsigned int baseIndex = static_cast<unsigned int>(vertices.size());
    
    int resolution = RESOLUTION >> m_level;
    if (resolution < 2) resolution = 2;
    
    glm::vec2 size = m_bottomRight - m_topLeft;
    
    for (int j = 0; j <= resolution; ++j) {
        for (int i = 0; i <= resolution; ++i) {
            float u = m_topLeft.x + (static_cast<float>(i) / resolution) * size.x;
            float v = m_topLeft.y + (static_cast<float>(j) / resolution) * size.y;
            
            glm::vec3 cubePos = GetCubePosition(u, v);
            glm::vec3 spherePos = CubeToSphere(cubePos);
            
            Vertex vertex;
            vertex.position = spherePos * radius;
            vertex.normal = spherePos;
            vertex.texCoord = glm::vec2(u, v);
            
            vertices.push_back(vertex);
        }
    }
    
    for (int j = 0; j < resolution; ++j) {
        for (int i = 0; i < resolution; ++i) {
            unsigned int topLeft = baseIndex + j * (resolution + 1) + i;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = topLeft + (resolution + 1);
            unsigned int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

CubeSphere::CubeSphere(float radius, int maxLevel) : m_radius(radius), m_maxLevel(maxLevel), m_VAO(0), m_VBO(0), m_EBO(0) {
    for (int i = 0; i < 6; ++i) {
        m_faces[i] = std::make_unique<QuadNode>(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0, static_cast<CubeFace>(i));
    }
    
    InitializeGL();
}

CubeSphere::~CubeSphere() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void CubeSphere::InitializeGL() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void CubeSphere::Update(const glm::vec3& cameraPos) {
    for (auto& face : m_faces) {
        if (face) {
            face->Update(cameraPos, m_radius, m_maxLevel);
        }
    }
    
    UpdateMesh();
}

void CubeSphere::UpdateMesh() {
    m_vertices.clear();
    m_indices.clear();
    
    for (auto& face : m_faces) {
        if (face) {
            face->GenerateMesh(m_vertices, m_indices, m_radius);
        }
    }
    
    // Mesh generated
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_DYNAMIC_DRAW);
}

void CubeSphere::Render() {
    if (m_indices.empty()) return;
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

int CubeSphere::GetTriangleCount() const {
    return static_cast<int>(m_indices.size() / 3);
}

int CubeSphere::GetActiveNodeCount() const {
    int nodeCount = 0;
    for (const auto& face : m_faces) {
        if (face) {
            face->CountNodes(nodeCount);
        }
    }
    return nodeCount;
}

void QuadNode::CountNodes(int& nodeCount) const {
    if (m_subdivided) {
        for (const auto& child : m_children) {
            if (child) {
                child->CountNodes(nodeCount);
            }
        }
    } else {
        nodeCount++;
    }
}