#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Primitive.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void generateCube(float size, glm::vec3 color, MeshData& meshData);
void generateSphere(float radius, int numSlices, glm::vec3 color, MeshData& meshData);
void generateCone(float radius, float height, int numSlices, glm::vec3 color, MeshData& meshData);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

float PI = 3.1415926535f;

int main()
{
    if (!glfwInit())
        return -1;
    
    //Force version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Must be called after glfwMakeContextCurrent
    if (glewInit()!=GLEW_OK) {
        std::cout << "GLEW Error!" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader = Shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    MeshData cubeMesh;
    generateCube(0.5f, glm::vec3(1.0f, 1.0f, 1.0f), cubeMesh);

    MeshData sphereMesh;
    generateSphere(0.5f, 10, glm::vec3(1.0f, 0.0f, 1.0f), sphereMesh);

    MeshData coneMesh;
    generateCone(0.5f, 1.0f, 10, glm::vec3(0.0f, 1.0f, 1.0f), coneMesh);

    Primitive* sphere = new Primitive(&sphereMesh);
    Primitive* cube = new Primitive(&cubeMesh);
    Primitive* cone = new Primitive(&coneMesh);

    //Before drawing, tell which shader to use and bind VAO
    shader.use();

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Move camera back
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view,glm::vec3(0.0f, 0.0f, -2.0f));

    float fov = 55.0f;
    glm::mat4 projection = glm::perspective(fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = (float)glfwGetTime();

        //Note that view and projection are shared across all our objects. There is only 1 camera!
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        //Draw transformed cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        cube->Draw();

        //Draw transformed sphere
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time * 0.2f, glm::vec3(0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        sphere->Draw();

        //Draw transformed cone
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        cone->Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete sphere;
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

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

    for (int i = 0; i < southPoleIndex; ++i)
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