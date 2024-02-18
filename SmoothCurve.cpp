#include "SmoothCurve.hpp"
#include <cmath>

uint32_t bisect(const std::vector<float> &values, float t)
{
    uint32_t low = 0;
    uint32_t high = values.size()-1;
    uint32_t mid = (low+high) / 2;
    while (high-low > 1) // t is not in [ t_i , t_{i+1} )
    {
        if (t >= values[mid])
            low = mid;
        else
            high = mid;
        mid = (low+high) / 2;
    }

    return low;
}

SmoothICurve::SmoothICurve(const CSVFile &csv,
    const std::vector<std::pair<std::string, float>> &columns,
    const std::pair<std::string, float> &time_or_scale,
    bool cyclic)
    : Mesh(csv, columns, cyclic ? GL_LINE_LOOP : GL_LINE_STRIP)
{
    if (csv.getRowCount() <= 2)
        return;

    time.reserve(csv.getRowCount());
    if (auto * time_col = csv.getColumn(time_or_scale.first))
    {
        // time_col exists
        t_start = std::stof(time_col->at(0));
        time.push_back(t_start);

        for (uint32_t r=1; r < csv.getRowCount()-1; r++)
            time.push_back(std::stof(time_col->at(r)));

        t_end = std::stof(time_col->at(csv.getRowCount()-1));
        time.push_back(t_end);
    }
    else
    {
        // parametrization with respect to arc length and scaling time_or_scale.second
        t_start = 0.0f;
        t_end = (csv.getRowCount()-1) * time_or_scale.second;
        for (uint32_t i=0; i < csv.getRowCount(); i++)
            time.push_back(i * time_or_scale.second);
    }

    /*
     * Calculates the momemtums of a natural/cyclic spline through the points of the CSV.
     */

    if (cyclic)
        {}//calculateCyclicSpline();
    else
        calculateNaturalSpline();
}

void SmoothICurve::calculateNaturalSpline()
{
    // Representing the differences in the time
    std::vector<float> h;

    // Representing tridiagonal symmetric momentum matrix (Upper, Diagonal)
    std::vector<float> u;
    std::vector<float> d;

    const uint32_t n = time.size()-1;
    d.resize(n-1);
    u.resize(n-2);

    h.resize(n);

    spline_M.resize(n+1);
    spline_M[0] = glm::fvec3(0.0f); // natural conditions, no curvature
    spline_M[n] = glm::fvec3(0.0f);

    h[0] = time[1] - time[0];
    for (uint32_t i=1; i < n-1; i++)
    {
        h[i] = time[i+1] - time[i];
        u[i-1] = h[i] / 6.0f;
        d[i-1] = (h[i-1] + h[i]) / 3.0f;
    }
    h[n-1] = time[n] - time[n-1];
    d[n-2] = (h[n-2] + h[n-1]) / 3.0f;

    for (uint32_t i=0; i < n-1; i++)
    {
        const glm::fvec3 pos_b(m_vertices[i*m_stride], m_vertices[i*m_stride + 1], m_vertices[i*m_stride + 2]);
        const glm::fvec3 pos_i(m_vertices[(i+1)*m_stride], m_vertices[(i+1)*m_stride + 1], m_vertices[(i+1)*m_stride + 2]);
        const glm::fvec3 pos_f(m_vertices[(i+2)*m_stride], m_vertices[(i+2)*m_stride + 1], m_vertices[(i+2)*m_stride + 2]);

        spline_M[i+1] = (pos_f - pos_i) / h[i+1] - (pos_i - pos_b) / h[i];
    }

    for (uint32_t i=1; i < n-1; i++)
    {
        const float ratio = u[i-1] / d[i-1];
        spline_M[i+1] -= ratio * spline_M[i];
        d[i] -= ratio * u[i-1];
    }

    spline_M[n-1] /= d[n-2];
    for (uint32_t i=n-2; i >= 1; i--)
    {
        spline_M[i] -= u[i-1] * spline_M[i+1];
        spline_M[i] /= d[i-1];
    }
}

glm::fvec3 SmoothICurve::operator()(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    const uint32_t low = bisect(time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = time[low];
    const float t1 = time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    const float inv_h6 = inv_h / 6.0f;

    return M1 * ((t-t0)*(t-t0)*(t-t0) * inv_h6) - M0 * ((t-t1)*(t-t1)*(t-t1) * inv_h6) + D*(t-t0) - C*(t-t1);
}

glm::fvec3 SmoothICurve::diff(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    const uint32_t low = bisect(time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = time[low];
    const float t1 = time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    const float inv_h2 = inv_h * 0.5f;

    return M1 * ((t-t0)*(t-t0) * inv_h2) - M0 * ((t-t1)*(t-t1) * inv_h2) + D - C;
}

glm::fvec3 SmoothICurve::diff2(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    const uint32_t low = bisect(time, t);
    const uint32_t high = low+1;

    const float t0 = time[low];
    const float t1 = time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    return M1 * ((t-t0) * inv_h) - M0 * ((t-t1) * inv_h);
}

void SmoothICurve::diffs(const float t, glm::fvec3 &T, glm::fvec3 &N) const {
    if (t < t_start || t > t_end)
    {
        T = glm::fvec3(0.0f);
        N = glm::fvec3(0.0f);
        return;
    }
    
    const uint32_t low = bisect(time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = time[low];
    const float t1 = time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;
    const float inv_h2 = inv_h * 0.5f;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    T = M1 * ((t-t0)*(t-t0) * inv_h2) - M0 * ((t-t1)*(t-t1) * inv_h2) + D - C;
    N = M1 * ((t-t0) * inv_h) - M0 * ((t-t1) * inv_h);
}

glm::fvec3 SmoothICurve::getTangent(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    return glm::normalize(diff(t));
}

glm::fvec3 SmoothICurve::getNormal(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    return glm::normalize(diff2(t));
}

glm::fvec3 SmoothICurve::getBinormal(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fvec3(0.0f);
    
    glm::fvec3 T, N;
    diffs(t, T, N);
    return glm::normalize(glm::cross(T, N));
}

glm::fmat3 SmoothICurve::getTNB(const float t) const {
    if (t < t_start || t > t_end)
        return glm::fmat3(1.0f);
    
    glm::fvec3 T, N;
    diffs(t, T, N);
    T = glm::normalize(T);
    N = glm::normalize(N);
    glm::fvec3 B = glm::cross(T, N);

    return glm::fmat3(T, N, B);
}