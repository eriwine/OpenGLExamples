#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

class Primitive {
public:
    Primitive(MeshData* meshData);
    ~Primitive();
    void Draw();
private:
    MeshData* m_meshData;
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
    size_t m_numIndices;
};