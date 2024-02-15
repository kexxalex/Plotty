#include <GL/glew.h>
#include "glWindow.hpp"
#include <iostream>


void render(Window &window);


int main()
{
    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }

    Window window = Window("Test");

    glClearColor(1.0, 0.6, 0.0, 1.0);
    glClearDepth(1.0);

    glfwSwapInterval(1);
    render(window);
    render(window);
    render(window);

    while (!window.shouldClose())
    {
        render(window);
        std::cout << window.getAvgFrameTime() << '\n';
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}


void render(Window &window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window.swap();
}