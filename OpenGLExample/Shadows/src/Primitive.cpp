#include "Primitive.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Primitive::Primitive(MeshData* meshData) : m_meshData(meshData)
{
    //Vertex Array Object
    glGenVertexArrays(1, &m_vao);

    //Create Vertex Buffer Object
    glGenBuffers(1, &m_vbo);

    glGenBuffers(1, &m_ebo);

    //Bind Vertex Array Object
    glBindVertexArray(m_vao);
    //Bind Vertex Buffer Object to VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    //Fill VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, meshData->vertices.size() * sizeof(Vertex), &meshData->vertices[0], GL_STATIC_DRAW);
    //Bind Element Buffer Object to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    //Fill EBO with index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indices.size() * sizeof(unsigned int), &meshData->indices[0], GL_STATIC_DRAW);

    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    //Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex,color)));
    glEnableVertexAttribArray(1);
    //Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    //UV
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));
    glEnableVertexAttribArray(3);

    m_numIndices = meshData->indices.size();
}

Primitive::~Primitive()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Primitive::Draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_numIndices, GL_UNSIGNED_INT, 0);
}
