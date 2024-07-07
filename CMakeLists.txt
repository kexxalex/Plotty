cmake_minimum_required(VERSION 3.5.0)
project(Plotty VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)


set(FILES
    plotty.cpp
    glWindow.cpp
    CSVReader.cpp
    Mesh.cpp
    SmoothCurve.cpp
    Shader.cpp)

add_executable(${PROJECT_NAME} ${FILES})

target_link_libraries(${PROJECT_NAME} GLEW GL glfw)