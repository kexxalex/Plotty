#pragma once


#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>

constexpr uint32_t N_FRAMETIMES = 4;
constexpr double INV_N_FRAMETIMES = 1.0 / N_FRAMETIMES;

class glWindow {
public:
    explicit glWindow( const std::string &title = "GL Window",
                       int width = 1280, int height = -1, bool fullscreen = false,
                       int gl_major = 4, int gl_minor = 6 );

    void swap() const noexcept;

    void makeCurrent() const noexcept { glfwMakeContextCurrent(m_window); }
    bool shouldClose() const noexcept { return glfwWindowShouldClose(m_window); }

    void setWidth( int width );

    void setHeight( int height );

    void setSize( int width, int height );

    constexpr int getWidth() const noexcept { return m_width; }
    constexpr int getHeight() const noexcept { return m_height; }

    constexpr GLFWwindow *getPointer() const noexcept { return m_window; }

    constexpr double getAvgFrameTime() const noexcept
    {
        double avg = 0.0;
        for (double m_frameTime : m_frameTimes)
            avg += m_frameTime;
        return avg * INV_N_FRAMETIMES;
    }

private:
    GLFWwindow *m_window;

    mutable double m_frameTimes[N_FRAMETIMES];
    mutable double m_lastFrame;
    mutable int m_frameTimeIndex;

    int m_width;
    int m_height;
};
