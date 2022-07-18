#pragma once
#include "Primitive.h"
#include <glm/glm.hpp>

const float PI = 3.1415926535f;
const float PI2 = PI * 2;

void generateCube(float size, glm::vec3 color, MeshData& meshData) {
    //VERTICES
    //-------------
    Vertex vertices[24] = {
        //Front face
        {glm::vec3(-size, -size, -size),color,glm::vec3(0.0f,0.0f,-1.0f)},
        {glm::vec3(-size, +size, -size),color,glm::vec3(0.0f,0.0f,-1.0f)},
        {glm::vec3(+size, +size, -size),color,glm::vec3(0.0f,0.0f,-1.0f)},
        {glm::vec3(+size, -size, -size),color,glm::vec3(0.0f,0.0f,-1.0f)},
        //Back face
        {glm::vec3(-size, -size, +size),color,glm::vec3(0.0f,0.0f,1.0f)},
        {glm::vec3(+size, -size, +size),color,glm::vec3(0.0f,0.0f,1.0f)},
        {glm::vec3(+size, +size, +size),color,glm::vec3(0.0f,0.0f,1.0f)},
        {glm::vec3(-size, +size, +size),color,glm::vec3(0.0f,0.0f,1.0f)},
        //Top face
        {glm::vec3(-size, +size, -size),color,glm::vec3(0.0f,1.0f,0.0f)},
        {glm::vec3(-size, +size, +size),color,glm::vec3(0.0f,1.0f,0.0f)},
        {glm::vec3(+size, +size, +size),color,glm::vec3(0.0f,1.0f,0.0f)},
        {glm::vec3(+size, +size, -size),color,glm::vec3(0.0f,1.0f,0.0f)},
        //Bottom face
        {glm::vec3(-size, -size, -size),color,glm::vec3(0.0f,-1.0f,0.0f)},
        {glm::vec3(+size, -size, -size),color,glm::vec3(0.0f,-1.0f,0.0f)},
        {glm::vec3(+size, -size, +size),color,glm::vec3(0.0f,-1.0f,0.0f)},
        {glm::vec3(-size, -size, +size),color,glm::vec3(0.0f,-1.0f,0.0f)},
        //Left face
        {glm::vec3(-size, -size, +size),color,glm::vec3(-1.0f,0.0f,0.0f)},
        {glm::vec3(-size, +size, +size),color,glm::vec3(-1.0f,0.0f,0.0f)},
        {glm::vec3(-size, +size, -size),color,glm::vec3(-1.0f,0.0f,0.0f)},
        {glm::vec3(-size, -size, -size),color,glm::vec3(-1.0f,0.0f,0.0f)},
        //Right face
        {glm::vec3(+size, -size, -size),color,glm::vec3(1.0f,0.0f,0.0f)},
        {glm::vec3(+size, +size, -size),color,glm::vec3(1.0f,0.0f,0.0f)},
        {glm::vec3(+size, +size, +size),color,glm::vec3(1.0f,0.0f,0.0f)},
        {glm::vec3(+size, -size, +size),color,glm::vec3(1.0f,0.0f,0.0f)}
    };
    meshData.vertices.assign(&vertices[0], &vertices[24]);

    //INDICES
    //-------------

    unsigned int indices[36] = {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        8, 9, 10,
        8, 10, 11,

        // bottom face
        12, 13, 14,
        12, 14, 15,

        // left face
        16, 17, 18,
        16, 18, 19,

        // right face
        20, 21, 22,
        20, 22, 23
    };
    meshData.indices.assign(&indices[0], &indices[36]);
}

void generateSphere(float radius, int numSlices, glm::vec3 color, MeshData& meshData) {

    //VERTICES
    //-------------

    Vertex topVertex = { glm::vec3(0,radius,0),color, glm::vec3(0,1,0)};
    meshData.vertices.push_back(topVertex);

    float phiStep = PI / (float)numSlices;
    float thetaStep = 2.0f * PI / numSlices;

    for (int i = 1; i <= numSlices - 1; ++i)
    {
        float phi = i * phiStep;

        // Vertices of ring.
        for (int j = 0; j <= numSlices; ++j)
        {
            float theta = j * thetaStep;

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            Vertex vertex = { glm::vec3(x, y, z), color, glm::normalize(glm::vec3(x, y, z))};
            meshData.vertices.push_back(vertex);

        }
    }
    Vertex bottomVertex = { glm::vec3(0, -radius, 0), color, glm::vec3(0,-1,0)};
    meshData.vertices.push_back(bottomVertex);

    //INDICES
    // -----------------------

    //Connects top pole to first ring
    for (int i = 1; i <= numSlices; ++i)
    {
        meshData.indices.push_back(0);
        meshData.indices.push_back(i + 1);
        meshData.indices.push_back(i);
    }

    unsigned int baseIndex = 1;
    unsigned int ringVertexCount = numSlices + 1;
    for (int i = 0; i < numSlices - 2; ++i)
    {
        for (int j = 0; j < numSlices; ++j)
        {
            meshData.indices.push_back(baseIndex + i * ringVertexCount + j);
            meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }
    unsigned int southPoleIndex = (unsigned int)meshData.vertices.size() - 1;

    // Offset the indices to the index of the first vertex in the last ring.
    baseIndex = southPoleIndex - ringVertexCount;

    for (unsigned int i = 0; i < southPoleIndex; ++i)
    {
        meshData.indices.push_back(southPoleIndex);
        meshData.indices.push_back(baseIndex + i);
        meshData.indices.push_back(baseIndex + i + 1);
    }
}
