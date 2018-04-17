#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak(); // Break debugging if x returns false
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__)); // Wrap a function with an error boundary

/**
Clear all (unrelated) previous errors.
*/
void GLClearError();

/**
Check for an error and log the error to the console.

@param function The function where the error happend
@param file The file where the error happend
@param line The line number where the error happend
@return Whether there was an error
*/
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};