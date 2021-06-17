#include "ShaderProgram.h"

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Read and compile shaders
    int success;
    char info[512];

    const std::string vertexText = read_file(vertexPath);
    const char* vertexTextPtr = vertexText.c_str();
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexTextPtr, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, info);
        std::cout << "Failed to compile vertex shader:\n" << info << "\n";
    }

    const std::string fragmentText = read_file(fragmentPath);
    const char* fragmentTextPtr = fragmentText.c_str();
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentTextPtr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, info);
        std::cout << "Failed to compile fragment shader:\n" << info << "\n";
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, info);
        std::cout << "Failed to link shaders:\n" << info << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(shaderProgram);
}

const unsigned int ShaderProgram::getId()
{
    return shaderProgram;
}

std::string ShaderProgram::read_file(const std::string& path)
{
    constexpr auto read_size = std::size_t{ 4096 };
    auto stream = std::ifstream{ path.data() };

    auto out = std::string{};
    auto buf = std::string(read_size, '\0');

    while (stream.read(&buf[0], read_size))
    {
        out.append(buf, 0, stream.gcount());
    }

    out.append(buf, 0, stream.gcount());
    return out;
}
