#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"
#include "Primitive.h"
#include "ShapeGen.h"
#include "FlyCamera.h"
#include "Camera.h"

void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* filePath);
unsigned int loadCubemap(std::vector<std::string> faces);
unsigned int createCubeVAO();
unsigned int createSkyboxVAO();
unsigned int createPlaneVAO();
unsigned int createQuadVAO();
void renderScene(const Shader& shader, float currentTime);

//Time 
float deltaTime = 0.0f;
float prevFrameTime = 0.0f;

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//Input
const float mouseSensitivity = 0.4f;
const float scrollSensitivity = 0.5f;
const bool mouseInvertX = false, mouseInvertY = false;
float prevMouseX = SCR_WIDTH / 2, prevMouseY = SCR_HEIGHT / 2;
bool firstMouse = true;

//Camera
Camera camera = Camera(glm::vec3(0), glm::vec3(0, 0, 1.0f), 60.0f, (float)SCR_WIDTH / SCR_HEIGHT);
FlyCamera cameraController = FlyCamera(&camera, 5.0f);

//Textures
GLuint skyboxTexture;
GLuint wallTexture;
GLuint grassTexture;

//Geometry
MeshData* cubeMesh;
Primitive* cubeRenderer;
MeshData* planeMesh;
Primitive* planeRenderer;
MeshData* quadMesh;
Primitive* quadRenderer;

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

    if (glewInit()!=GLEW_OK) {
        std::cout << "GLEW Error!" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    //Input callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //Hide + lock cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Shader skyboxShader = Shader("shaders/skybox.vert", "shaders/skybox.frag");

    Shader litShader = Shader("shaders/defaultLit.vert", "shaders/defaultLit.frag");

    Shader debugDepthShader = Shader("shaders/debugDepth.vert", "shaders/debugDepth.frag");

    Shader renderToDepthShader = Shader("shaders/renderToDepth.vert", "shaders/renderToDepth.frag");

    std::vector<std::string> faces{
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg",
    };

    skyboxTexture = loadCubemap(faces);
    wallTexture = loadTexture("textures/wall.jpg");
    grassTexture = loadTexture("textures/Grass_Color.jpg");

    //Create geometry
    cubeMesh = new MeshData();
    createCube(1.0f, 1.0f, 1.0f, glm::vec3(1.0f), cubeMesh);
    cubeRenderer = new Primitive(cubeMesh);

    planeMesh = new MeshData();
    createPlane(1.0f, 1.0f, glm::vec3(1.0f), planeMesh);
    planeRenderer = new Primitive(planeMesh);

    quadMesh = new MeshData();
    createQuad(2.0f, 2.0f, glm::vec3(1.0f), quadMesh);
    quadRenderer = new Primitive(quadMesh);

    //Create depth buffer
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOWMAP_WIDTH = 2048, SHADOWMAP_HEIGHT = 2048;

    //Create depth texture
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Attach depth texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE); //Tell OpenGL we aren't rendering to color buffer
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Create framebuffer for drawing depth to screen
    GLuint screenFBO;
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    GLuint screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //Timing
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - prevFrameTime;
        prevFrameTime = currentTime;

        //Match viewport to shadowmap resolution
        glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = glm::vec3(cos(currentTime * 0.5) * 5.0, 5.0, sin(currentTime * 0.5)*5.0);

        //1. Draw geometry from light POV
        glCullFace(GL_FRONT); //Use front face culling when rendering to depth map

        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightTransform = lightProjection * lightView;
        renderToDepthShader.use();
        renderToDepthShader.setMat4("u_lightSpaceMatrix", lightTransform);
        renderScene(renderToDepthShader, currentTime);
      
        //Draw to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //2. Draw geometry from camera POV
        {
            glCullFace(GL_BACK);

            litShader.use();
            litShader.setMat4("u_lightSpaceMatrix", lightTransform);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, wallTexture);
            litShader.setInt("u_texture", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            litShader.setInt("u_skyboxTexture", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            litShader.setInt("u_shadowMap", 2);

            litShader.setMat4("u_projection", camera.GetProjectionMatrix());
            litShader.setMat4("u_view", camera.GetViewMatrix());
            litShader.setVec3("u_cameraPos", camera.GetPosition());
            litShader.setVec3("u_lightColor", glm::vec3(1.0));
            litShader.setVec3("u_lightPos", lightPos);

            renderScene(litShader, currentTime);
        }

        //Draw light position as cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        litShader.setMat4("u_model", model);
        cubeRenderer->Draw();

        //Draw skybox
        {
            glDepthFunc(GL_LEQUAL);
            glCullFace(GL_FRONT);

            skyboxShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            skyboxShader.setInt("u_texture", 0);
            skyboxShader.setMat4("u_view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
            skyboxShader.setMat4("u_projection", camera.GetProjectionMatrix());

            cubeRenderer->Draw();
        }


        //Draw depth buffer directly to screen
        {
            //glDisable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);

            debugDepthShader.use();
            debugDepthShader.setFloat("near_plane", 0.01f);
            debugDepthShader.setFloat("far_plane", 15.0f);

            glm::vec3 scale = glm::vec3(0.25f);
            scale.y *= ((float)SCR_WIDTH / SCR_HEIGHT);
            debugDepthShader.setVec3("scale", scale);
            debugDepthShader.setVec3("offset", glm::vec3(-0.75f,1.0f - scale.y,0.0f));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthTexture);

            quadRenderer->Draw();
        }
       

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderScene(const Shader& shader, float currentTime)
{
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    //Draw cubes
    shader.setVec2("u_tile", glm::vec2(0.5f));

    //Spinning cube 1 
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, currentTime * 0.2f, glm::vec3(-0.5, 0.2f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setMat4("u_model", model);
    cubeRenderer->Draw();

    //Spinning cube 2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.0f));
    model = glm::rotate(model, currentTime * 0.4f, glm::vec3(0.0, 0.2f, 0.5f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setMat4("u_model", model);
    cubeRenderer->Draw();

    //Spinning cube 3
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5f, 1.5f, 1.0f));
    model = glm::rotate(model, currentTime * 0.3f, glm::vec3(0.0, 0.2f, 0.5f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setMat4("u_model", model);
    cubeRenderer->Draw();

    //Wall 1
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 0.5f));
    shader.setMat4("u_model", model);
    shader.setVec2("u_tile", glm::vec2(2.0f));

    cubeRenderer->Draw();

    //Wall 2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.5f, -2.0f));
    model = glm::scale(model, glm::vec3(2.0f, 3.0f, 0.5f));
    shader.setMat4("u_model", model);
    shader.setVec2("u_tile", glm::vec2(2.0f,3.0f));

    //glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeRenderer->Draw();

    //Ground plane
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("u_model", model);
    shader.setFloat("u_tile", 5.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    shader.setInt("u_texture", 0);
    planeRenderer->Draw();
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
unsigned int loadTexture(const char* filePath)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return 0;
    }
    stbi_image_free(data);
    return texture;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (size_t i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            printf("Failed to load texture from file %s", faces[i].c_str());
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.SetFov(camera.GetFov() - (float)yOffset * scrollSensitivity);
}
