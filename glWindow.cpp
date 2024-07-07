#include "glWindow.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>

static void sizeCallback( GLFWwindow *window, int width, int height )
{
    glWindow *const win = static_cast<glWindow *>(glfwGetWindowUserPointer(window));
    if (win)
        win->setSize(width, height);
}


glWindow::glWindow( const std::string &title, int width, int height, bool fullscreen, int gl_major, int gl_minor )
    : m_window(nullptr)
    , m_frameTimes{ }
    , m_lastFrame(glfwGetTime())
    , m_frameTimeIndex(0)
    , m_width(width)
    , m_height(height)
{
    if (m_height == -1)
        m_height = static_cast<int>(ceil(m_width * (9.0 / 16.0)));


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    // Enable 10-Bit Colors, if available
    glfwWindowHint(GLFW_RED_BITS, 10);
    glfwWindowHint(GLFW_GREEN_BITS, 10);
    glfwWindowHint(GLFW_BLUE_BITS, 10);
    glfwWindowHint(GLFW_ALPHA_BITS, 2);

    GLFWmonitor *const monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), monitor, nullptr);

    if (nullptr == m_window) {
        throw std::runtime_error("GLFW Window could not be created");
    }

    makeCurrent();

    std::cout << "GL context version: " << gl_major << '.' << glGetString(GL_VERSION) << '\n';

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


void glWindow::setWidth( const int width )
{
    setSize(width, m_height);
}

void glWindow::setHeight( const int height )
{
    setSize(m_width, height);
}

void glWindow::setSize( const int width, const int height )
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);
}
