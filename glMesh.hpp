#pragma once


#include <GL/glew.h>
#include "CSVReader.hpp"

class Mesh
{
public:
    Mesh(Mesh &&mesh);
    Mesh(const Mesh &mesh);

    Mesh(const CSVFile &csv,
         const std::vector<std::pair<std::string, float>> &columns,
         GLenum mode);

    void push();

    void render() const;

private:
    GLuint m_vaoID;
    GLuint m_vboID;
    GLenum m_mode;
    uint32_t m_stride;
    uint32_t m_length;
    std::vector<float> m_vertices;
};
