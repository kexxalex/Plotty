cmake_minimum_required(VERSION 3.5.0)
project(Plotty VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)


set(FILES
    glad/glad.c
    plotty.cpp
    glWindow.cpp
    CSVReader.cpp
    Mesh.cpp
    Shader.cpp
    Interpolation/SmoothICurve.cpp
)


link_directories(${PROJECT_SOURCE_DIR}/libs)
add_executable(${PROJECT_NAME} ${FILES})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/modules/glfw/include)
target_link_libraries(${PROJECT_NAME} GL glfw3)