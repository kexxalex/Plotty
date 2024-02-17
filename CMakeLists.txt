cmake_minimum_required(VERSION 3.5.0)
project(Plotty VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 17)


set(FILES plotty.cpp glWindow.cpp CSVReader.cpp glMesh.cpp Shader.cpp)

add_executable(${PROJECT_NAME} ${FILES})

target_include_directories(${PROJECT_NAME} PRIVATE external/glfw/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/glew/include)

target_link_directories(${PROJECT_NAME}
    PRIVATE libs
)

target_link_libraries(${PROJECT_NAME} GLEW GL glfw3)
