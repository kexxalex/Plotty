#include "glWindow.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>

static void sizeCallback(GLFWwindow* window, int width, int height)
{
    glWindow* const win = static_cast<glWindow*>(glfwGetWindowUserPointer(window));
    if (win)
        win->setSize(width, height);
}


glWindow::glWindow(const std::string &title, int width, int height, bool fullscreen)
    : m_window(nullptr)
    , m_frameTimes{}
    , m_lastFrame(glfwGetTime())
    , m_frameTimeIndex(0)
    , m_width(width)
    , m_height(height)
{
    if (m_height == -1)
        m_height = int( ceil(m_width * (9.0/16.0)) );
    
    // Using OpenGL 4.6 for direct state access
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    // Enable 10-Bit Colors, if available
    glfwWindowHint(GLFW_RED_BITS, 10);
    glfwWindowHint(GLFW_GREEN_BITS, 10);
    glfwWindowHint(GLFW_BLUE_BITS, 10);
    glfwWindowHint(GLFW_ALPHA_BITS, 2);

    GLFWmonitor* const monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), monitor, nullptr);

    if (nullptr == m_window)
    {
        throw std::runtime_error( "GLFW Window could not be created" );
    }

    makeCurrent();

    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, m_width, m_height);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, sizeCallback);
}



void glWindow::swap() const noexcept
{
    const double t = glfwGetTime();
    m_frameTimeIndex %= N_FRAMETIMES;
    m_frameTimes[m_frameTimeIndex++] = t - m_lastFrame;
    m_lastFrame = t;

    glfwSwapBuffers(m_window);
}


void glWindow::setWidth(int width)
{
    setSize(width, m_height);
}

void glWindow::setHeight(int height)
{
    setSize(m_width, height);
}

void glWindow::setSize(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);
}
