#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;
const unsigned int NUM_PARTICLES = 100;
const unsigned int VERTEX_STRIDE = 6;
const unsigned int VERTEX_BUFFER_SIZE = NUM_PARTICLES * VERTEX_STRIDE;
float vertices[VERTEX_BUFFER_SIZE];

float deltaTime, lastFrameTime;

unsigned int VBO;

float randomRange(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min); 
}

void randomizeParticles() {

    for (size_t i = 0; i < NUM_PARTICLES; i++)
    {
        unsigned int startIndex = i * VERTEX_STRIDE;
        vertices[startIndex] = randomRange(-0.5, 0.5);
        vertices[startIndex + 1] = randomRange(-0.5, 0.5);
        vertices[startIndex + 2] = 0;
        vertices[startIndex + 3] = 1.0f;
        vertices[startIndex + 4] = 1.0f;
        vertices[startIndex + 5] = 1.0f;
    }
}

int main()
{
    if (!glfwInit())
        return -1;

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

    Shader shader = Shader("shaders/vertex.glsl", "shaders/geometry.geom", "shaders/fragment.glsl");

    randomizeParticles();

    //Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //Create Vertex Buffer Object
    glGenBuffers(1, &VBO);


    //Bind Vertex Array Object
    glBindVertexArray(VAO);
    //Bind Vertex Buffer Object to VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Fill VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    //Set vertex attributes pointers.
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Colors starting at offset of 12 bytes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
   
    //Before drawing, tell which shader to use and bind VAO
    shader.use();
    glBindVertexArray(VAO);

    shader.setFloat("size", 0.1f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        processInput(window);

        //Draw
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
        
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        randomizeParticles();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}