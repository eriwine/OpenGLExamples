#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//Cube positions
float vertices[] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, +0.5f, -0.5f,
    +0.5f, +0.5f, -0.5f,
    +0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, +0.5f,
    -0.5f, +0.5f, +0.5f,
    +0.5f, +0.5f, +0.5f,
    +0.5f, -0.5f, +0.5f
};

unsigned int indices[] = {
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

glm::mat4 rotateX(float a) {
    return glm::mat4(
        1, 0, 0, 0,
        0, cos(a), -sin(a), 0,
        0, sin(a), cos(a), 0,
        0, 0, 0, 1
    );
}

glm::mat4 rotateY(float a) {
    return glm::mat4(
        cos(a), 0, sin(a), 0,
        0, 1, 0, 0,
        -sin(a), 0, cos(a), 0,
        0, 0, 0, 1
    );
}
glm::mat4 rotateZ(float a) {
    return glm::mat4(
        cos(a), -sin(a), 0, 0,
        sin(a), cos(a), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

glm::mat4 translate(glm::vec3 pos) {
    return glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        pos.x, pos.y, pos.z, 1
    );
}

glm::mat4 scale(glm::vec3 scale) {
    return glm::mat4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    );
}

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

    //Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //Create Vertex Buffer Object
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    //Bind Vertex Array Object
    glBindVertexArray(VAO);
    //Bind Vertex Buffer Object to VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Fill VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Bind Element Buffer Object to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Fill EBO with index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Set vertex attributes pointers.
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
   
    //Before drawing, tell which shader to use and bind VAO
    shader.use();
    glBindVertexArray(VAO);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 view = glm::mat4(1.0f);
    view = translate(glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = (float)glfwGetTime();

        glm::mat4 model = glm::mat4(1.0f);
        model = translate(glm::vec3(0.5f,0.0f,0)) * rotateZ(time * 0.5f) * rotateY(0) * rotateX(time) * scale(glm::vec3(0.5f,0.5f,0.5f)) * model;

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        //Draw geometry
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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