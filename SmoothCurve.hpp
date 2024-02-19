#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>


class SmoothICurve : public Mesh {
public:
    SmoothICurve(const CSVFile &csv,
        const std::vector<std::pair<std::string, float>> &columns,
        const std::pair<std::string, float> &time_and_scale,
        bool cyclic = false);
    

    glm::fvec3 at(float t) const override;
    glm::fvec3 diffAt(float t) const override;
    glm::fvec3 diff2At(float t) const override;
    void diffs(float t, glm::fvec3 &T, glm::fvec3 &N) const;

    glm::fvec3 getTangent(float t) const;
    glm::fvec3 getNormal(float t) const;
    glm::fvec3 getBinormal(float t) const;
    glm::fmat4 getOrthonormalFrame(float t) const override;

    glm::fvec3 at(const glm::fvec2 &uv) const override { return at(uv.x); }
    glm::fvec3 diffAt(const glm::fvec2 &uv) const override { return diffAt(uv.x); }
    glm::fvec3 diff2At(const glm::fvec2 &uv) const override { return diff2At(uv.x); }
    glm::fmat4 getOrthonormalFrame(const glm::fvec2 &uv) const override { return getOrthonormalFrame(uv.x); }

    glm::fvec4 transform(float t, const glm::fvec4 &direction) const override;
    glm::fvec4 transform(const glm::fvec2 &uv, const glm::fvec4 &direction) const override { return transform(uv.x, direction); }

protected:
    SmoothICurve() = delete;

    void calculateCyclicSpline();
    void calculateNaturalSpline();

    float t_start, t_end;
    std::vector<float> m_time;
    std::vector<glm::fvec3> spline_M;
    bool m_cyclic;
};