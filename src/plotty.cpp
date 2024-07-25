#define GLM_ENABLE_EXPERIMENTAL
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#endif

// #define GLAD_GL_IMPLEMENTATION
#include <glad.h>

#include "GUI/glWindow.hpp"
#include <iostream>
#include "IO/CSVReader.hpp"
#include "3D/Interpolation/SmoothICurve.hpp"
#include "Rendering/Shader.hpp"
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>


static void render( glWindow &window, const glm::fmat4 &MVP, Shader &shader, const std::vector<Mesh *> &meshes )
{
    shader.setMatrixFloat4("MVP", MVP);
    shader.Bind();

    for (const Mesh *mesh : meshes)
        mesh->render();
}


Mesh createGridPlane( const int S, const f32 spacing = 1.0f )
{
    std::vector<f32> vertices;
    for (int i = -S; i < S; i++) {
        vertices.push_back(i * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(-S * spacing);

        vertices.push_back(i * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(S * spacing);

        vertices.push_back(-S * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(i * spacing);

        vertices.push_back(S * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(i * spacing);


        vertices.push_back(i * spacing);
        vertices.push_back(-S * spacing);
        vertices.push_back(0.0f);

        vertices.push_back(i * spacing);
        vertices.push_back(S * spacing);
        vertices.push_back(0.0f);

        vertices.push_back(-S * spacing);
        vertices.push_back(i * spacing);
        vertices.push_back(0.0f);

        vertices.push_back(S * spacing);
        vertices.push_back(i * spacing);
        vertices.push_back(0.0f);


        vertices.push_back(0.0f);
        vertices.push_back(i * spacing);
        vertices.push_back(-S * spacing);

        vertices.push_back(0.0f);
        vertices.push_back(i * spacing);
        vertices.push_back(S * spacing);

        vertices.push_back(0.0f);
        vertices.push_back(-S * spacing);
        vertices.push_back(i * spacing);

        vertices.push_back(0.0f);
        vertices.push_back(S * spacing);
        vertices.push_back(i * spacing);
    }

    return {std::move(vertices), 3, GL_LINES};
}


void run( glWindow &window )
{
    std::vector<Mesh *> meshes;
    CSVFile circleCSV("res/meshes/geodesicSphere.csv");
    if (!circleCSV.read(','))
        return;

    CSVFile spiralCSV("res/meshes/spiral.csv");
    if (!spiralCSV.read(','))
        return;

    CSVFile onfCSV("res/meshes/ONF.csv");
    if (!onfCSV.read(','))
        return;

    const std::vector<std::pair<std::string, f32>> columns = { { "X", 0.0f }, { "Y", 0.0f }, { "Z", 0.0f }, { "T", 1.0f } };

    SmoothICurve circle(circleCSV, columns, { "T", 1.0f }, true);
    meshes.push_back(&circle);

    SmoothICurve spiral(spiralCSV, { "T", 1.0f }, { "X", 0.0f }, { "Y", 0.0f }, { "Z", 0.0f }, &circle, true);
    meshes.push_back(&spiral);

    Mesh tbnSpiral(onfCSV, { "T", 1.0f }, { "X", 0.0f }, { "Y", 0.0f }, { "Z", 0.0f }, &spiral, GL_LINES);
    meshes.push_back(&tbnSpiral);

    for (Mesh *const mesh : meshes)
        mesh->push();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);

    glfwSwapInterval(1);

    Shader cartesian("./res/shader/cartesian");
    Shader cartesianSystem("./res/shader/cartesianSystem");
    Mesh cartesianSystemGrid = createGridPlane(32, 0.5f);
    cartesianSystemGrid.push();

    constexpr glm::fvec3 up(0.0f, 1.0f, 0.0f);

    glm::dvec3 position(3.0, 3.0, 3.0);
    glm::dvec2 rotation(M_PI_4, -M_PI_4);
    glm::dvec2 lastPosition;
    bool leftClick = false;

    GLFWwindow *const winPtr = window.getPointer();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    while (!window.shouldClose()) {
        const glm::dmat4 proj = glm::perspectiveFov<double>(glm::radians(45.0), window.getWidth(), window.getHeight(), 0.03, 1024.0);
        constexpr glm::dvec3 X = { 1.0f, 0.0f, 0.0f };
        constexpr glm::dvec3 Y = { 0.0f, 1.0f, 0.0f };

        position.x = sin(glfwGetTime() * 0.125) * M_SQRT1_2 * 3.0;
        position.y = M_SQRT1_2 * 3.0;
        position.z = cos(glfwGetTime() * 0.125) * M_SQRT1_2 * 3.0;
        rotation.x = glfwGetTime() * 0.125;

        const glm::dmat4 ROT = glm::rotate(-rotation.y, X) * glm::rotate(-rotation.x, Y);

        glm::fmat4 MVP = proj * ROT * glm::translate(-position);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glDisable(GL_DEPTH_TEST);
        glLineWidth(1.0f);
        cartesianSystem.setMatrixFloat4("MVP", MVP);
        cartesianSystem.Bind();
        cartesianSystemGrid.render();

        glClear(GL_DEPTH_BUFFER_BIT);
        glLineWidth(4.0f);
        render(window, MVP, cartesian, meshes);
        window.swap();
        glfwPollEvents();
    }
}


int main()
{
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return 1;
    }

    glWindow window("Test");

    const int version = gladLoadGL(glfwGetProcAddress);
    printf("GL Version %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    run(window);

    glfwTerminate();

    return 0;
}
