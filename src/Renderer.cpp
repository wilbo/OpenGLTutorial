#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    GLenum error = glGetError();

    if (error)
    {
        std::cout << "[OpenGL Error] (" << std::hex << error << ") in " << function << " at " << file << " on line " << std::dec << line << std::endl;
        return false;
    }

    return true;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    // Bind everything so we can draw
    shader.Bind();
    va.Bind();
    ib.Bind();
    
    // Draw the current selected buffer
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr)); // nullptr, because the indices are bound to the current buffer: GL_ELEMENT_ARRAY_BUFFER
}