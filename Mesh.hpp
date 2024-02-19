#pragma once


#include <GL/glew.h>
#include "CSVReader.hpp"
#include <glm/glm.hpp>

class Mesh
{
public:
    Mesh(Mesh &&mesh);
    Mesh(const Mesh &mesh);

    Mesh(const CSVFile &csv,
         const std::vector<std::pair<std::string, float>> &columns,
         GLenum mode);

    Mesh(const CSVFile &csv,
         const std::pair<std::string, float> &T,
         const std::pair<std::string, float> &X,
         const std::pair<std::string, float> &Y,
         const std::pair<std::string, float> &Z,
         const Mesh * curve,
         GLenum mode);

    Mesh(std::vector<float> &&positions,
         uint32_t stride,
         GLenum mode);

    void push();

    void render() const;

    /*
     * An orthonormal basis for the local coordiantes local.
     * If only 1-dim, local.x is used as time t for the curve c(t)
     * Binormal is interpreted as height for curves, this yields, that
     * a planar curve lies in span{Tangent, Normal}.
     */
    virtual glm::fmat4 getOrthonormalFrame(float t) const { return glm::fmat4(1.0f); };
    virtual glm::fmat4 getOrthonormalFrame(const glm::fvec2 &local) const { return glm::fmat4(1.0f); };

    virtual glm::fvec4 transform(float t, const glm::fvec4 &direction) const { return glm::fvec4(0.0f); }
    virtual glm::fvec4 transform(const glm::fvec2 &local, const glm::fvec4 &direction) const { return glm::fvec4(0.0f); }

    virtual glm::fvec3 at(float t) const { return glm::fvec3(0.0f); }
    virtual glm::fvec3 diffAt(float t) const { return glm::fvec3(0.0f); }
    virtual glm::fvec3 diff2At(float t) const { return glm::fvec3(0.0f); }

    virtual glm::fvec3 at(const glm::fvec2 &local) const { return glm::fvec3(0.0f); }
    virtual glm::fvec3 diffAt(const glm::fvec2 &local) const { return glm::fvec3(0.0f); }
    virtual glm::fvec3 diff2At(const glm::fvec2 &local) const { return glm::fvec3(0.0f); }

protected:
    Mesh() = delete;
    GLuint m_vaoID;
    GLuint m_vboID;
    GLenum m_mode;
    uint32_t m_stride;
    uint32_t m_length;
    std::vector<float> m_vertices;
};
