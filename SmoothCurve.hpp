#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>


class SmoothICurve : public Mesh {
public:
    SmoothICurve(const CSVFile &csv,
        const std::vector<std::pair<std::string, float>> &columns,
        const std::pair<std::string, float> &time_or_scale,
        bool cyclic = false);
    

    glm::fvec3 operator()(const float t) const;
    glm::fvec3 diff(const float t) const;
    glm::fvec3 diff2(const float t) const;
    void diffs(const float t, glm::fvec3 &T, glm::fvec3 &N) const;

    glm::fvec3 getTangent(const float t) const;
    glm::fvec3 getNormal(const float t) const;
    glm::fvec3 getBinormal(const float t) const;
    glm::fmat3 getTNB(const float t) const;


protected:
    SmoothICurve() = delete;

    // void calculateCyclicSpline();
    void calculateNaturalSpline();

    float t_start, t_end;
    std::vector<float> time;
    std::vector<glm::fvec3> spline_M;

};