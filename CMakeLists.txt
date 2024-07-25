cmake_minimum_required(VERSION 3.5.0)
project(Plotty LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# Adding GLFW for window management
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/modules/glfw ${CMAKE_BINARY_DIR}/glfw)


# Files for IMGUI support
set(IMGUI
        external/modules/imgui/imgui.cpp
        external/modules/imgui/imgui_draw.cpp
        external/modules/imgui/backends/imgui_impl_glfw.cpp
        external/modules/imgui/backends/imgui_impl_opengl3.cpp
        external/modules/imgui/imgui_tables.cpp
        external/modules/imgui/imgui_widgets.cpp
)


# Files for GLAD
set(GLAD
    external/glad/glad.c
    external/glad/glad.h
    external/glad/khrplatform.h
)


set(FILES
    src/plotty.cpp
    src/defines.hpp
    src/IO/CSVReader.cpp
    src/IO/CSVReader.hpp
    src/GUI/glWindow.cpp
    src/GUI/glWindow.hpp
    src/Rendering/Shader.cpp
    src/Rendering/Shader.hpp
    src/3D/Mesh.cpp
    src/3D/Mesh.hpp
    src/3D/Interpolation/SmoothICurve.cpp
    src/3D/Interpolation/SmoothICurve.hpp
)


include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/external/glad)
include_directories(${PROJECT_SOURCE_DIR}/external/modules/imgui)
include_directories(${PROJECT_SOURCE_DIR}/external/modules/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/external/modules/glm)

add_executable(${PROJECT_NAME} ${GLAD} ${FILES} ${IMGUI})


if(WIN32)
    target_link_libraries(${PROJECT_NAME} OpenGL32 glfw)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} GL glfw)
endif()
