#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>


class SmoothICurve : public Mesh {
public:
    SmoothICurve(const CSVFile &csv,
        const std::vector<std::pair<std::string, float>> &columns,
        const std::pair<std::string, float> &time_and_scale,
        bool cyclic = false);
    

    glm::fvec3 operator()(float t) const;
    glm::fvec3 diff(float t) const;
    glm::fvec3 diff2(float t) const;
    void diffs(float t, glm::fvec3 &T, glm::fvec3 &N) const;

    glm::fvec3 getTangent(float t) const;
    glm::fvec3 getNormal(float t) const;
    glm::fvec3 getBinormal(float t) const;
    glm::fmat3 getTNB(float t) const;


protected:
    SmoothICurve() = delete;

    void calculateCyclicSpline();
    void calculateNaturalSpline();

    float t_start, t_end;
    std::vector<float> time;
    std::vector<glm::fvec3> spline_M;
    bool m_cyclic;
};