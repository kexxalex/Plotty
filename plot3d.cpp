#include <GL/glew.h>
#include "glWindow.hpp"
#include <iostream>
#include "CSVReader.hpp"
#include "glMesh.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>


static void render(glWindow &window, Shader &shader, const std::vector<Mesh> &meshes)
{
    static const glm::fvec3 up(0.0f, 1.0f, 0.0f);
    static const glm::fvec3 center(0.0f, 0.0f, 0.0f);
    static const glm::fmat4 proj = glm::perspective(45.0f, 16.f/9.f, 0.03f, 1024.0f);

    const glm::fvec3 eye(cos(glfwGetTime()), 0.0f, sin(glfwGetTime()));

    glm::fmat4 view = glm::lookAt(eye*4.0f, center, up);
    glm::fmat4 MVP = proj * view;

    shader.setMatrixFloat4("MVP", MVP);
    shader.Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (const Mesh &mesh : meshes)
        mesh.render();
    window.swap();
}


int main()
{

    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }

    glWindow window("Test");

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Could not initialize GLEW" << std::endl;
        glfwTerminate();
        return 2;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    std::vector<Mesh> meshes;
    CSVFile csv("geodesicSphere.csv");
    csv.read(',');

    const std::vector<std::pair<std::string, float>> columns = {{"X", 0.0f}, {"Y", 0.0f}, {"Z", 0.0f}};
    meshes.emplace_back(csv, columns, GL_LINE_LOOP);
    for (Mesh &mesh : meshes)
        mesh.push();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);

    glfwSwapInterval(1);

    Shader cartesian("./res/shader/cartesian");

    while (!window.shouldClose())
    {
        render(window, cartesian, meshes);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}



