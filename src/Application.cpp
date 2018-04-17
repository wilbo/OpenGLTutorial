#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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

        VertexArray va; // Initialize our vertex array 

        VertexBuffer vb(positions, 4 * 2 * sizeof(float)); // Create and bind a buffer for the vertices

        VertexBufferLayout layout; // Create a layout for the buffer we created
        layout.Push<float>(2);

        va.AddBuffer(vb, layout);
        
        IndexBuffer ib(indices, 6); // Create and bind a buffer for the indices
       
        Shader shader("res/shaders/Basic.shader");

        va.Unbind();
        shader.Unbind();
        vb.Unbind();
        ib.Unbind();

        Renderer renderer;

        // Animation stuff
        float r = 0.0f;
        float increment = 0.05f;

        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(va, ib, shader);

            // Animate the r value between 0.0 and 1.0
            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;
            r += increment;

            GLCall(glfwSwapBuffers(window)); // Swap front and back buffers
            GLCall(glfwPollEvents()); // Poll for and process events
        }
    }

    glfwTerminate();
    return 0;
}