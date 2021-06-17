#pragma once
#include <string>

class ShaderProgram
{
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();
    const unsigned int getId();

private:
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    std::string read_file(const std::string& path);
};
