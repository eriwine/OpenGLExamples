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
#include "FlyCamera.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//Time 
float deltaTime = 0.0f;
float prevFrameTime = 0.0f;

//Input
const float mouseSensitivity = 0.4f;
const float scrollSensitivity = 0.5f;
const bool mouseInvertX = false, mouseInvertY = false;
float prevMouseX = SCR_WIDTH/2, prevMouseY = SCR_HEIGHT/2;
bool firstMouse = true;

//Camera
Camera camera = Camera(glm::vec3(0), glm::vec3(0, 0, 1.0f), 60.0f, (float)SCR_WIDTH / SCR_HEIGHT);
FlyCamera cameraController = FlyCamera(&camera, 5.0f);

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    Shader shader = Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader lightShader = Shader("shaders/vertex.glsl", "shaders/lightFragment.glsl");

    MeshData cubeMesh;
    generateCube(0.5f, glm::vec3(1,0,1), cubeMesh);

    MeshData sphereMesh;
    generateSphere(0.5f, 20, glm::vec3(0,1,1), sphereMesh);

    Primitive sphere = Primitive(&sphereMesh);
    Primitive cube = Primitive(&cubeMesh);


    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = camera.GetProjectionMatrix();


    //Light
    glm::vec3 lightPos = glm::vec3(0, 1, 0);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.5f);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - prevFrameTime;
        prevFrameTime = currentTime;

        lightPos = glm::vec3(cos(currentTime) * 2, 1, sin(currentTime)*2);
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
        shader.setVec3("eyePos", camera.GetPosition());

        //Draw transformed cube
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentTime * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        cube.Draw();

        //Draw transformed sphere
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentTime * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setMat4("model", model);
        sphere.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

float getInputAxis(GLFWwindow* window, int positiveButton, int negativeButton) {
    float axis = 0.0f;
    if (glfwGetKey(window, negativeButton) == GLFW_PRESS) {
        axis -= 1.0f;
    }
    if (glfwGetKey(window, positiveButton) == GLFW_PRESS) {
        axis += 1.0f;
    }
    return axis;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float horizontalAxis = getInputAxis(window, GLFW_KEY_D, GLFW_KEY_A);
    float forwardAxis = getInputAxis(window, GLFW_KEY_W, GLFW_KEY_S);
    float upAxis = getInputAxis(window, GLFW_KEY_E, GLFW_KEY_Q);

    cameraController.ProcessMovement(horizontalAxis, forwardAxis, upAxis, deltaTime);
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

    cameraController.ProcessLook(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.SetFov(camera.GetFov() -(float)yOffset * scrollSensitivity);
}
