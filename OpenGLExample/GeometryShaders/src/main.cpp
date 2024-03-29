#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"

struct Vec3 {
    float x, y, z;
    Vec3() :x(0), y(0), z(0) {};
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {};
};

struct Vec4 {
    float x, y, z, w;
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
};

struct Vertex {
    Vec3 position = Vec3(0,0,0);
    Vec4 color = Vec4(1,1,1,1);
};

struct Particle {
    Vec3 position;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;
const unsigned int NUM_PARTICLES = 100;
const unsigned int VERTEX_STRIDE = 7;
const unsigned int VERTEX_BUFFER_SIZE = NUM_PARTICLES * VERTEX_STRIDE;

Vertex vertices[NUM_PARTICLES];
Particle particles[NUM_PARTICLES];

float deltaTime, lastFrameTime;

unsigned int quadVBO;

float randomRange(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min); 
}

void randomizeParticles() {

    for (unsigned int i = 0; i < NUM_PARTICLES; i++)
    {
        vertices[i].position = { randomRange(-0.5, 0.5), randomRange(-0.5,0.5), 0.0 };
        vertices[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    }
   // memcpy(vertexBuffer, vertices, NUM_PARTICLES * sizeof(Vertex));
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
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
    glGenBuffers(1, &quadVBO);


    //Bind Vertex Array Object
    glBindVertexArray(VAO);
    //Bind Vertex Buffer Object to VAO
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    //Fill VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    //Set vertex attributes pointers.
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Colors starting at offset of 12 bytes
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3*sizeof(float)));
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

        for (unsigned int i = 0; i < NUM_PARTICLES; i++)
        {
            float x = vertices[i].position.x;
            x += deltaTime * 0.5;
            if (x > 1)
                x = -1;
            vertices[i].position.x = x;
        }

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

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
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}