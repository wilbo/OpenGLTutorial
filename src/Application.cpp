#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Initialize Glew */
    if (glewInit() != GLEW_OK)
        cout << "glewInit error!" << endl;

    /* Log the OpenGL version used because we can */
    cout << glGetString(GL_VERSION) << endl;


    /* 
        Create and select the data & buffer for drawing
    */

    float positions[6] = {
        -0.5, -0.5,
        0.0, 0.5,
        0.5, -0.5
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer); // Generate a single buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // Select the buffer to be drawn
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); // Add the data to the buffer

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw the triangle */
        // At the moment, this draws nothing because it doesn't specify anything about 
        // the current selected buffer. OpenGL doesn't know yet how to draw this.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}