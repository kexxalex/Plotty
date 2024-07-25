#pragma once


#include <glad.h>
#include "IO/CSVReader.hpp"
#include <glm/glm.hpp>

class Mesh {
public:
    // Why would you create a mesh without points?
    Mesh() = delete;

    // Copies a mesh
    Mesh( const Mesh &mesh );

    // Moves a mesh
    Mesh( Mesh &&mesh ) noexcept;

    Mesh( const CSVFile &csv,
          const std::vector<std::pair<std::string, f32>> &columns,
          GLenum mode );

    /**
     * @param csv a CSVFile object
     * @param T name of the column used as time and its scaling or uniform delta_t
     * @param X name of the column used as x coordinates or default x coordinate
     * @param Y name of the column used as y coordinates or default y coordinate
     * @param Z name of the column used as z coordinates or default z coordinate
     * @param mesh use (x,y,z) and t as local coordinates along this Mesh pointer
     * @param mode draw mode for OpenGL
     */
    Mesh( const CSVFile &csv,
          const std::pair<std::string, f32> &T,
          const std::pair<std::string, f32> &X,
          const std::pair<std::string, f32> &Y,
          const std::pair<std::string, f32> &Z,
          const Mesh *mesh,
          GLenum mode );

    Mesh( std::vector<f32> &&positions, u32 stride, GLenum mode );

    virtual ~Mesh() = default;

    void push();

    void render() const;

    /**
     * An orthonormal basis for the local coordiantes local.<br>
     * If only 1-dim, local.x is used as time t for the curve c(t)
     * Binormal is interpreted as height for curves, this yields, that
     * a planar curve lies in span{Tangent, Normal}.
     */
    virtual glm::fmat4 getOrthonormalFrame( f32 t ) const;

    /**
     * An orthonormal basis for the local coordiantes local.<br>
     * If only 1-dim, local.x is used as time t for the curve c(t)
     * Binormal is interpreted as height for curves, this yields, that
     * a planar curve lies in span{Tangent, Normal}.
     */
    virtual glm::fmat4 getOrthonormalFrame( const glm::fvec2 &local ) const;

    virtual glm::fvec4 transform( f32 t, const glm::fvec4 &direction ) const;

    virtual glm::fvec4 transform( const glm::fvec2 &local, const glm::fvec4 &direction ) const;

    virtual glm::fvec3 at( f32 t ) const;

    virtual glm::fvec3 diffAt( f32 t ) const;

    virtual glm::fvec3 diff2At( f32 t ) const;

    virtual glm::fvec3 at( const glm::fvec2 &local ) const;

    virtual glm::fvec3 diffAt( const glm::fvec2 &local ) const;

    virtual glm::fvec3 diff2At( const glm::fvec2 &local ) const;

protected:
    std::vector<f32> m_vertices;
    GLuint m_vaoID;
    GLuint m_vboID;
    GLenum m_mode;
    u32 m_stride;
    u32 m_length;
    GLsizeiptr m_bufferSize;
};
