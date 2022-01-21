#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    // Program ID
    GLuint _ID;

    // Constructor reads vertex and fragment shader parths
    Shader(const char* vertexPath, const char* fragmentPath);

    // Use the shader
    void use();

    // Utility functions to set the values of Uniform variables
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 &value);
};

#endif