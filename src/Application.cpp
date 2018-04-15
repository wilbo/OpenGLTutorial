#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

/**
    A struct that combines shader sources into a single type.
*/
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

/**
    Parse a single shader file to a shader source.

    @param filePath Path to the shader file
    @return ShaderProgramSource The combined shader sources in as ShaderProgramSource
*/
static ShaderProgramSource ParseShader(const std::string filePath)
{
    std::ifstream stream(filePath);

    enum class Shadertype
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2]; // 2 for vertex and fragment shaders
    Shadertype type = Shadertype::NONE;

    while (getline(stream, line))
    {
        // Set the type if #shader is found
        if (line.find("#shader") != std::string::npos) 
        {
            if (line.find("vertex") != std::string::npos)
                type = Shadertype::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = Shadertype::FRAGMENT;
        }
        else // Add the line to the source
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

/**
    Compiles a single shader so it can be attached.

    @param type The type of the shader
    @param source The source code of the shader as string
    @return unsigned int An identifier for the compiled shader
*/
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // Create the shader
    const char* src = source.c_str(); // Return the pointer of the first character of the source
    GLCall(glShaderSource(id, 1, &src, nullptr)); // Specify the shader source code
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result)); // Returns the compile status parameter
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) alloca(length * sizeof(char)); // Allocate this on the stack dynamically because 'char message[length]' is not allowed
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

/**
    Links the given shaders into a single shader so that it can be bound.

    @param vertexShader Source of the vertexshader as string
    @param fragmentShader Source of the fragmentshader as string
    @return unsigned int An identifier for the newly created shader
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // Create a shader program to attach shader to
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach both shaders to the program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    
    GLCall(glLinkProgram(program)); // Link the program so the shaders are used
    GLCall(glValidateProgram(program)); // Check if the program can be executed

    // The shaders are linked to the progam, so the shaders can be deleted
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Use the core profile to enforce our own implementation of a vertex array
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Synchronize the refresh rate with our native refresh rate
    glfwSwapInterval(1);
    
    // Initialize Glew
    if (glewInit() != GLEW_OK)
        std::cout << "glewInit error!" << std::endl;

    // Log the OpenGL version used because we can
    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        // Create and select (bind) the data & buffer for drawing
        float positions[] =
        {
            -0.5, -0.5, // bottom-left
             0.5, -0.5, // bottom right
             0.5,  0.5, // top right
            -0.5,  0.5  // top left
        };

        // The indexes of the vertices we want to draw
        unsigned int indices[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        // Initialize our vertex array object
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        // Create and bind a buffer for the vertices
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        // Create a layout for the buffer we created
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); // links the buffer with the vao

        // Create and bind a buffer for the indices
        IndexBuffer ib(indices, 6);

        // Creating the shaders
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

        // Set the color of every pixel using uniforms
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        float r = 0.0f;
        float increment = 0.05f;

        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            GLCall(glClear(GL_COLOR_BUFFER_BIT)); // Render here

            GLCall(glUseProgram(shader)); // bind the shader
            GLCall(glUniform4f(location, r, 0.3, 0.8, 1.0)); // Set the color in the shader with the use of a uniform
            GLCall(glBindVertexArray(vao)); // bind the vertex array (vertex buffer and layout)
            ib.Bind(); // bind the indices

            // Draw the current selected buffer
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // nullptr, because the indices are bound to the current buffer: GL_ELEMENT_ARRAY_BUFFER

            // Animate the r value between 0.0 and 1.0
            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;
            r += increment;

            GLCall(glfwSwapBuffers(window)); // Swap front and back buffers
            GLCall(glfwPollEvents()); // Poll for and process events
        }

        GLCall(glDeleteProgram(shader)); // Clean up the the program with shaders
    }

    glfwTerminate();
    return 0;
}