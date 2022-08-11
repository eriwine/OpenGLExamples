#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    //Uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
private:
    //Program ID in openGL
    unsigned int m_id;
    unsigned int getUniformLocation(const std::string& name) const;
};

#endif