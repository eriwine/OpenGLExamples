#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* filePath);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

float vertices[] = {
     //Positions          //Colors           //Texture Coords (UVs)
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

unsigned int indices[] = {
    0,1,3,
    1,2,3
};

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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int texture = loadTexture("textures/wall.jpg");

    Shader shader = Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    shader.setInt("Texture", 0);
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
    //Bind Index Buffer Object to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Fill EBO with vertex data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //Set vertex attributes pointers.
    //Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Colors starting at offset of 12 bytes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    //Texture coordinates at offset of 24 bytes
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
   
    //Before drawing, tell which shader to use and bind VAO
    shader.use();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);

    //Render loop
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw geometry
        shader.setFloat("OffsetX", time/5.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

unsigned int loadTexture(const char* filePath)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}