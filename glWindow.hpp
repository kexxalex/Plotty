#pragma once


#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>

constexpr uint32_t N_FRAMETIMES = 4;
constexpr double INV_N_FRAMETIMES = 1.0 / N_FRAMETIMES;

class glWindow {
public:
    glWindow(const std::string &title="GL Window", int width=1280, int height=-1, bool fullscreen=false);

    void swap() const noexcept;
    inline void makeCurrent() const noexcept { glfwMakeContextCurrent(m_window); }
    inline bool shouldClose() const noexcept { return glfwWindowShouldClose(m_window); }

    void setWidth(int width);
    void setHeight(int height);
    void setSize(int width, int height);

    constexpr int getWidth() const noexcept { return m_width; }
    constexpr int getHeight() const noexcept { return m_height; }

    constexpr GLFWwindow* getPointer() const noexcept { return m_window; }

    constexpr double getAvgFrameTime() const noexcept
    {
        double avg = 0.0;
        for (uint32_t i=0; i < N_FRAMETIMES; i++)
            avg += m_frameTimes[i];
        return avg * INV_N_FRAMETIMES;
    }

private:
    GLFWwindow* m_window;

    mutable double m_frameTimes[N_FRAMETIMES];
    mutable double m_lastFrame;
    mutable int m_frameTimeIndex;

    int m_width;
    int m_height;
};
