#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Primitive.h"
#include "ShapeGen.h"
#include "OrbitCamera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//Camera
const float mouseSensitivity = 0.01f;
const float scrollSensitivity = 0.1f;
const bool mouseInvertX = false, mouseInvertY = true;
float prevMouseX = SCR_WIDTH/2, prevMouseY = SCR_HEIGHT/2;
bool firstMouse = true;

OrbitCamera camera = OrbitCamera(glm::vec3(0, 0, 0), 45, 3.0f, 6.0f);

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    Shader shader = Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader lightShader = Shader("shaders/vertex.glsl", "shaders/lightFragment.glsl");

    MeshData cubeMesh;
    generateCube(0.5f, glm::vec3(1,0,1), cubeMesh);

    MeshData sphereMesh;
    generateSphere(0.5f, 20, glm::vec3(0,1,1), sphereMesh);

    Primitive sphere = Primitive(&sphereMesh);
    Primitive cube = Primitive(&cubeMesh);


    glEnable(GL_DEPTH_TEST);

    camera.setAzimuth(PI / 2);
    glm::mat4 projection = glm::perspective(camera.getFov(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //Light
    glm::vec3 lightPos = glm::vec3(0, 1, 0);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.5f);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        float time = (float)glfwGetTime();

        lightPos = glm::vec3(cos(time) * 2, 1, sin(time)*2);
        //Use lightShader to render light
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);

        //Draw light as small sphere at lightPos
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightShader.setMat4("model", model);
        sphere.Draw();

        //Switch to our shader for rest of the shapes
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("eyePos", camera.getEyePos());

        //Draw transformed cube
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        cube.Draw();

        //Draw transformed sphere
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, time * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        sphere.Draw();

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xPosf = (float)xpos, yPosf = (float)ypos;
    if (firstMouse)
    {
        prevMouseX = xPosf;
        prevMouseY = yPosf;
        firstMouse = false;
    }
    float xoffset = xPosf - prevMouseX;
    float yoffset = prevMouseY - yPosf;
    prevMouseX = xPosf;
    prevMouseY = yPosf;

    xoffset *= mouseSensitivity * (mouseInvertX ? -1 : 1);
    yoffset *= mouseSensitivity * (mouseInvertY ? -1 : 1);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        camera.rotate(xoffset, yoffset);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.zoom(-(float)yOffset * scrollSensitivity);
}
