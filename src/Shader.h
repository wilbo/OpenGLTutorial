#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

/**
A struct that combines shader sources into a single type.
*/
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filePath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform1i(const std::string& name, int value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, glm::mat4& matrix);
private:

    /**
        Parse a single shader file to a shader source.

        @param filePath Path to the shader file
        @return ShaderProgramSource The combined shader sources in as ShaderProgramSource
    */
    ShaderProgramSource ParseShader(const std::string filePath);

    /**
        Links the given shaders into a single shader so that it can be bound.

        @param vertexShader Source of the vertexshader as string
        @param fragmentShader Source of the fragmentshader as string
        @return unsigned int An identifier for the newly created shader
    */
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    /**
        Compiles a single shader so it can be attached.

        @param type The type of the shader
        @param source The source code of the shader as string
        @return unsigned int An identifier for the compiled shader
    */
    unsigned int CompileShader(unsigned int type, const std::string& source);

    /**
        Return the location of the shader based on uniform name

        @param name The name of the uniform
        @return The location
    */
    unsigned int GetUniformLocation(const std::string& name);
};