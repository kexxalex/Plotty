#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>


class SmoothICurve : public Mesh {
public:
    SmoothICurve() = delete;

    /**
     * Creates a 3D spline curve.
     * @param csv a CSVFile for the coordinates
     * @param columns for drawing. First three are used as space and 4th as time coordinates
     * @param time_and_scale time column or delta-t
     * @param cyclic
     */
    SmoothICurve( const CSVFile &csv,
                  const std::vector<std::pair<std::string, f32>> &columns,
                  const std::pair<std::string, f32> &time_and_scale,
                  bool cyclic = false );

    /**
     * Creates a 3D spline curve along <mesh>.
     * @param csv a CSVFile for the coordinates
     * @param columns for drawing. First three are used as space and 4th as time coordinates
     * @param time_and_scale time column or delta-t
     * @param mesh transform to its local orthonormal frame
     * @param cyclic
     */
    SmoothICurve( const CSVFile &csv,
                  const std::vector<std::pair<std::string, f32>> &columns,
                  const std::pair<std::string, f32> &time_and_scale,
                  const Mesh * mesh,
                  bool cyclic = false );


    /**
    * Evaluates the curve at the given time.
    * @param t time
    * @return position
    */
    glm::fvec3 at( f32 t ) const override;

    /**
    * Evaluates the first derivative of the curve at the given time.
    * @param t time
    * @return position
    */
    glm::fvec3 diffAt( f32 t ) const override;

    /**
    * Evaluates the second derivative of the curve at the given time.
    * @param t time
    * @return velocity
    */
    glm::fvec3 diff2At( f32 t ) const override;

    /**
    * Evaluates the first and second derivative of the curve at the given time.
    * @param t time
    * @param T reference of the Tangent
    * @param N reference of the Normal
    */
    void diffs( f32 t, glm::fvec3 &T, glm::fvec3 &N ) const;

    /**
    * @param t time
    * @return Normalized tangent vector at time t
    */
    glm::fvec3 getTangent( f32 t ) const;

    /**
    * @param t time
    * @return Normalized normal vector at time t
    */
    glm::fvec3 getNormal( f32 t ) const;

    /**
    * @param t time
    * @return Normalized binormal vector at time t
    */
    glm::fvec3 getBinormal( f32 t ) const;

    /**
    * Creates a orthogonal matrix out of T,N,B vectors and combines it with the translation transformation.<br>
    * This matrix M transforms from local coordinates (x,y,z) to world coordinates (x',y',z') and preserves orthogonality.<br>
    * A point (in affine coords) p = (x,y,z, 1) can be transformed by M*p.<br>
    * A vector v = (x,y,z,0) can be transformed from local coordinates to world by M*v.
    * @param t time
    * @return affine orthogonal transform matrix
    */
    glm::fmat4 getOrthonormalFrame( f32 t ) const override;

    glm::fvec3 at( const glm::fvec2 &uv ) const override { return at(uv.x); }
    glm::fvec3 diffAt( const glm::fvec2 &uv ) const override { return diffAt(uv.x); }
    glm::fvec3 diff2At( const glm::fvec2 &uv ) const override { return diff2At(uv.x); }
    glm::fmat4 getOrthonormalFrame( const glm::fvec2 &uv ) const override { return getOrthonormalFrame(uv.x); }

    glm::fvec4 transform( f32 t, const glm::fvec4 &direction ) const override;

    glm::fvec4 transform( const glm::fvec2 &uv, const glm::fvec4 &direction ) const override { return transform(uv.x, direction); }

protected:
    void calculateCyclicSpline();

    void calculateNaturalSpline();

    std::vector<f32> m_time;
    std::vector<glm::fvec3> spline_M;
    f32 t_start, t_end;
    bool m_cyclic;
};
