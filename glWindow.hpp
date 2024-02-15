#pragma once


#include <GLFW/glfw3.h>
#include <string>


constexpr int __N_FRAMETIMES = 4;


class Window {
public:
    Window(const std::string &title="GL Window", int width=1280, int height=-1, bool fullscreen=false);

    void swap() const noexcept;
    inline void makeCurrent() const noexcept { glfwMakeContextCurrent(m_window); }
    inline bool shouldClose() const noexcept { return glfwWindowShouldClose(m_window); }
    constexpr double getAvgFrameTime() const noexcept
    {
        double avg = 0.0;
        for (int i=0; i < __N_FRAMETIMES; i++)
            avg += m_frameTimes[i];
        return avg / __N_FRAMETIMES;
    }

    void setWidth(int width);
    void setHeight(int height);
    void setSize(int width, int height);

private:
    GLFWwindow* m_window;

    mutable double m_frameTimes[__N_FRAMETIMES];
    mutable double m_lastFrame;
    mutable int m_frameTimeIndex;
    int m_width;
    int m_height;
};