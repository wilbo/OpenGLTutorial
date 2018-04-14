#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
    glShaderSource(id, 1, &src, nullptr); // Specify the shader source code
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // Returns the compile status parameter
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char)); // Allocate this on the stack dynamically because 'char message[length]' is not allowed
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
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
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    glLinkProgram(program); // Link the program so the shaders are used
    glValidateProgram(program); // Check if the program can be executed

    // The shaders are linked to the progam, so the shaders can be deleted
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Initialize Glew
    if (glewInit() != GLEW_OK)
        std::cout << "glewInit error!" << std::endl;

    // Log the OpenGL version used because we can
    std::cout << glGetString(GL_VERSION) << std::endl;


    // Create and select (bind) the data & buffer for drawing
    float positions[] = {
        -0.5, -0.5,
         0.5, -0.5,
         0.5,  0.5,
        -0.5,  0.5
    };

    // The indexes of the vertices we want to draw
    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); // Generate a single buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // Select the buffer to be drawn
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW); // Add the data to the buffer

    // Create a layout for the buffer we created
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // Create and bind a buffer for the indices
    unsigned int ibo; // index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Creating the shaders
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT); // Render here

        // Draw the current selected buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // nullptr, because the indices are bound to the current buffer: GL_ELEMENT_ARRAY_BUFFER

        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents(); // Poll for and process events
    }

    glDeleteProgram(shader); // Clean up the the program with shaders
    glfwTerminate();
    return 0;
}