#pragma once
#include "Primitive.h"
#include <glm/glm.hpp>

const float PI = 3.1415926535f;
const float PI2 = PI * 2;

void generateCube(float size, glm::vec3 color, MeshData& meshData) {
    //VERTICES
    //-------------
    Vertex vertices[8] = {
        {glm::vec3(-size, -size, -size),color},
        {glm::vec3(-size, +size, -size),color},
        {glm::vec3(+size, +size, -size),color},
        {glm::vec3(+size, -size, -size),color},
        {glm::vec3(-size, -size, +size),color},
        {glm::vec3(-size, +size, +size),color},
        {glm::vec3(+size, +size, +size),color},
        {glm::vec3(+size, -size, +size),color}
    };
    meshData.vertices.assign(&vertices[0], &vertices[8]);

    //INDICES
    //-------------

    unsigned int indices[36] = {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };
    meshData.indices.assign(&indices[0], &indices[36]);
}

void generateSphere(float radius, int numSlices, glm::vec3 color, MeshData& meshData) {

    //VERTICES
    //-------------

    Vertex topVertex = { glm::vec3(0,radius,0), color };
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

            Vertex vertex = { glm::vec3(x, y, z), color };
            meshData.vertices.push_back(vertex);
        }
    }
    Vertex bottomVertex = { glm::vec3(0, -radius, 0), color };
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

void generateCone(float radius, float height, int numSlices, glm::vec3 color, MeshData& meshData) {

    //VERTICES
    //---------------

    unsigned int baseIndex = 0;
    float baseY = -0.5f * height;
    float topY = 0.5f * height;
    float theta = 2.0f * PI / numSlices;
    for (int i = 0; i <= numSlices; ++i)
    {
        float x = radius * cosf(i * theta);
        float z = radius * sinf(i * theta);
        Vertex vertex = { glm::vec3(x, baseY, z), color };
        meshData.vertices.push_back(vertex);
    }
    //Center of bottom
    Vertex centerVertex = { glm::vec3(0, baseY, 0), color };
    meshData.vertices.push_back(centerVertex);

    //Top
    Vertex topVertex = { glm::vec3(0, topY, 0), color };
    meshData.vertices.push_back(topVertex);

    //INDICES
    //----------
    unsigned int centerIndex = (unsigned int)meshData.vertices.size() - 2;
    unsigned int topIndex = (unsigned int)meshData.vertices.size() - 1;

    for (int i = 0; i < numSlices; ++i)
    {
        meshData.indices.push_back(centerIndex);
        meshData.indices.push_back(baseIndex + i);
        meshData.indices.push_back(baseIndex + i + 1);

        meshData.indices.push_back(topIndex);
        meshData.indices.push_back(baseIndex + i);
        meshData.indices.push_back(baseIndex + i + 1);
    }

}


void generateTorus(float outRadius, float inRadius, int outFacetsNum, int inFacetsNum, glm::vec3 color, MeshData& meshData) {

    float radius = outRadius; //Large disc radius 
    float ringRadius = outRadius - inRadius; //Radius of the tube ring

    int numVerticesPerRow = inFacetsNum + 1;
    int numVerticesPerColumn = outFacetsNum + 1;

    int numVertices = numVerticesPerRow * numVerticesPerColumn;

    float theta = 0.0f;
    float phi = 0.0f;

    float verticalAngularStride = PI2 / outFacetsNum;
    float horizontalAngularStride = PI2 / inFacetsNum;

    for (int verticalIt = 0; verticalIt < numVerticesPerColumn; verticalIt++)
    {
        theta = verticalAngularStride * verticalIt;
        for (int horizontalIt = 0; horizontalIt < numVerticesPerRow; horizontalIt++)
        {
            phi = horizontalAngularStride * horizontalIt;
            //position
            float x = cos(theta) * (radius + ringRadius * cos(phi));
            float y = sin(theta) * (radius + ringRadius * cos(phi));
            float z = ringRadius * sin(phi);

            Vertex vertex = { glm::vec3(x,y,z), color };
            meshData.vertices.push_back(vertex);
        }
    }
    int numIndices = inFacetsNum * outFacetsNum * 6;

    //INDICES
    //------------

    for (int verticalIt = 0; verticalIt < outFacetsNum; verticalIt++)
    {
        for (int horizontalIt = 0; horizontalIt < inFacetsNum; horizontalIt++)
        {
            unsigned int lt = horizontalIt + verticalIt * numVerticesPerRow;
            unsigned int rt = (horizontalIt + 1) + verticalIt * numVerticesPerRow;
            unsigned int lb = horizontalIt + (verticalIt + 1) * numVerticesPerRow;
            unsigned int rb = (horizontalIt + 1) + (verticalIt + 1) * numVerticesPerRow;

            meshData.indices.push_back(lt);
            meshData.indices.push_back(rt);
            meshData.indices.push_back(lb);

            meshData.indices.push_back(rt);
            meshData.indices.push_back(rb);
            meshData.indices.push_back(lb);
        }
    }
}