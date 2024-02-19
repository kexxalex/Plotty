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
    const std::pair<std::string, float> &time_and_scale,
    bool cyclic)
    : Mesh(csv, columns, cyclic ? GL_LINE_LOOP : GL_LINE_STRIP)
    , m_cyclic(cyclic)
{
    if (csv.getRowCount() <= 2)
        return;

    m_time.resize(csv.getRowCount());
    if (auto * time_col = csv.getColumn(time_and_scale.first))
    {
        // time_col exists
        t_start = std::stof(time_col->at(0)) * time_and_scale.second;
        m_time[0] = t_start;

        for (uint32_t r=1; r < csv.getRowCount()-1; r++)
            m_time[r] = std::stof(time_col->at(r)) * time_and_scale.second;

        t_end = std::stof(time_col->at(csv.getRowCount()-1)) * time_and_scale.second;
        m_time[csv.getRowCount()-1] = t_end;
    }
    else
    {
        // parametrization with respect to arc length and scaling time_or_scale.second
        t_start = 0.0f;
        t_end = (csv.getRowCount()-1) * time_and_scale.second;
        for (uint32_t i=0; i < csv.getRowCount(); i++)
            m_time[i] = i * time_and_scale.second;
    }

    /*
     * Calculates the momemtums of a natural/cyclic spline through the points of the CSV.
     */

    if (cyclic)
        calculateCyclicSpline();
    else
        calculateNaturalSpline();
}


void SmoothICurve::calculateCyclicSpline()
{
    // Representing the differences in the time
    std::vector<float> h;

    // Representing tridiagonal symmetric momentum matrix (Diagonal, last row)
    std::vector<float> d;
    std::vector<float> bottom;

    const uint32_t n = m_time.size()-1;
    d.resize(n-1);
    bottom.assign(n, 0.0f);

    h.resize(n);

    spline_M.resize(n+1);

    h[0] = m_time[1] - m_time[0];
    for (uint32_t i=1; i < n; i++)
    {
        h[i] = m_time[i+1] - m_time[i];
        d[i-1] = (h[i-1] + h[i]) / 3.0f;
    }

    // Equation for M_0 == M_n
    bottom[0] = h[0] / 6.0f;
    bottom[n-1] = (h[n-1] + h[0]) / 3.0f; // h[0] == "h[n]"
    bottom[n-2] = h[n-1] / 6.0f;

    // pos[n] has to be equal to pos[0]
    for (uint32_t i=1; i < n; i++)
    {
        const glm::fvec3 pos_b(m_vertices[(i-1)*m_stride], m_vertices[(i-1)*m_stride + 1], m_vertices[(i-1)*m_stride + 2]);
        const glm::fvec3 pos_i(m_vertices[i*m_stride], m_vertices[i*m_stride + 1], m_vertices[i*m_stride + 2]);
        const glm::fvec3 pos_f(m_vertices[(i+1)*m_stride], m_vertices[(i+1)*m_stride + 1], m_vertices[(i+1)*m_stride + 2]);

        spline_M[i] = (pos_f - pos_i) / h[i] - (pos_i - pos_b) / h[i-1];
    }
    const glm::fvec3 pos_b(m_vertices[(n-1)*m_stride], m_vertices[(n-1)*m_stride + 1], m_vertices[(n-1)*m_stride + 2]);
    const glm::fvec3 pos_i(m_vertices[0], m_vertices[1], m_vertices[2]);
    const glm::fvec3 pos_f(m_vertices[m_stride], m_vertices[m_stride + 1], m_vertices[m_stride + 2]);

    spline_M[n] = (pos_f - pos_i) / h[0] - (pos_i - pos_b) / h[n-1];

    for (uint32_t i=1; i < n-1; i++)
    {
        const float ratio = h[i] / 6.0f / d[i-1];
        spline_M[i+1] -= ratio * spline_M[i];
        d[i] -= ratio * h[i] / 6.0f;

        const float ratio_bot = bottom[i-1] / d[i-1];
        bottom[i-1] = 0.0f;
        bottom[i] -= ratio_bot * h[i] / 6.0f;
        spline_M[n] -= ratio_bot * spline_M[i];
    }

    const float ratio_bot = bottom[n-2]/d[n-2];
    bottom[n-2] = 0.0f;
    spline_M[n] -= ratio_bot * spline_M[n-1];

    spline_M[n] /= bottom[n-1];
    spline_M[0] = spline_M[n];

    spline_M[n-1] /= d[n-2];
    for (uint32_t i=n-2; i >= 1; i--)
    {
        spline_M[i] -= h[i] / 6.0f * spline_M[i+1];
        spline_M[i] /= d[i-1];
    }
}


void SmoothICurve::calculateNaturalSpline()
{
    // Representing the differences in the time
    std::vector<float> h;

    // Representing tridiagonal symmetric momentum matrix (Diagonal)
    std::vector<float> d;

    const uint32_t n = m_time.size()-1;
    d.resize(n-1);

    h.resize(n);

    spline_M.resize(n+1);
    spline_M[0] = glm::fvec3(0.0f); // natural conditions, no curvature
    spline_M[n] = glm::fvec3(0.0f);

    h[0] = m_time[1] - m_time[0];
    for (uint32_t i=1; i < n; i++)
    {
        h[i] = m_time[i+1] - m_time[i];
        d[i-1] = (h[i-1] + h[i]) / 3.0f;
    }

    for (uint32_t i=1; i < n; i++)
    {
        const glm::fvec3 pos_b(m_vertices[(i-1)*m_stride], m_vertices[(i-1)*m_stride + 1], m_vertices[(i-1)*m_stride + 2]);
        const glm::fvec3 pos_i(m_vertices[i*m_stride], m_vertices[i*m_stride + 1], m_vertices[i*m_stride + 2]);
        const glm::fvec3 pos_f(m_vertices[(i+1)*m_stride], m_vertices[(i+1)*m_stride + 1], m_vertices[(i+1)*m_stride + 2]);

        spline_M[i] = (pos_f - pos_i) / h[i] - (pos_i - pos_b) / h[i-1];
    }

    for (uint32_t i=1; i < n-1; i++)
    {
        const float ratio = h[i] / 6.0f / d[i-1];
        spline_M[i+1] -= ratio * spline_M[i];
        d[i] -= ratio * h[i] / 6.0f;
    }

    spline_M[n-1] /= d[n-2];
    for (uint32_t i=n-2; i >= 1; i--)
    {
        spline_M[i] -= h[i] / 6.0f * spline_M[i+1];
        spline_M[i] /= d[i-1];
    }
}

glm::fvec3 SmoothICurve::at(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    const uint32_t low = bisect(m_time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = m_time[low];
    const float t1 = m_time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    const float inv_h6 = inv_h / 6.0f;

    return M1 * ((t-t0)*(t-t0)*(t-t0) * inv_h6) - M0 * ((t-t1)*(t-t1)*(t-t1) * inv_h6) + D*(t-t0) - C*(t-t1);
}

glm::fvec3 SmoothICurve::diffAt(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    const uint32_t low = bisect(m_time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = m_time[low];
    const float t1 = m_time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    const float inv_h2 = inv_h * 0.5f;

    return M1 * ((t-t0)*(t-t0) * inv_h2) - M0 * ((t-t1)*(t-t1) * inv_h2) + D - C;
}

glm::fvec3 SmoothICurve::diff2At(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    const uint32_t low = bisect(m_time, t);
    const uint32_t high = low+1;

    const float t0 = m_time[low];
    const float t1 = m_time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    return M1 * ((t-t0) * inv_h) - M0 * ((t-t1) * inv_h);
}

void SmoothICurve::diffs(float t, glm::fvec3 &T, glm::fvec3 &N) const {
    if (!m_cyclic && (t < t_start || t > t_end))
    {
        T = glm::fvec3(0.0f);
        N = glm::fvec3(0.0f);
        return;
    }
    t = fmodf(t, t_end);
    
    const uint32_t low = bisect(m_time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = m_time[low];
    const float t1 = m_time[high];

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

glm::fvec3 SmoothICurve::getTangent(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    return glm::normalize(diffAt(t));
}

glm::fvec3 SmoothICurve::getNormal(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    return glm::normalize(diff2At(t));
}

glm::fvec3 SmoothICurve::getBinormal(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fvec3(0.0f);
    t = fmodf(t, t_end);
    
    glm::fvec3 T, N;
    diffs(t, T, N);
    N -= glm::dot(T, N)*T;
    N = glm::normalize(N);
    return glm::normalize(glm::cross(T, N));
}

glm::fmat4 SmoothICurve::getOrthonormalFrame(float t) const {
    if (!m_cyclic && (t < t_start || t > t_end))
        return glm::fmat4(1.0f);
    t = fmodf(t, t_end);
    
    const uint32_t low = bisect(m_time, t);
    const uint32_t high = low+1;
    
    const glm::fvec3 y0(m_vertices[low*m_stride], m_vertices[low*m_stride + 1], m_vertices[low*m_stride + 2]);
    const glm::fvec3 y1(m_vertices[high*m_stride], m_vertices[high*m_stride + 1], m_vertices[high*m_stride + 2]);

    const float t0 = m_time[low];
    const float t1 = m_time[high];

    const float h = t1 - t0;
    const float inv_h = 1.0f / h;
    const float inv_h6 = inv_h / 6.0f;
    const float inv_h2 = inv_h * 0.5f;

    const glm::fvec3 &M0 = spline_M[low];
    const glm::fvec3 &M1 = spline_M[high];

    const glm::fvec3 C = y0*inv_h - M0*(h/6.0f);
    const glm::fvec3 D = y1*inv_h - M1*(h/6.0f);

    const glm::fvec3 P = M1 * ((t-t0)*(t-t0)*(t-t0) * inv_h6) - M0 * ((t-t1)*(t-t1)*(t-t1) * inv_h6) + D*(t-t0) - C*(t-t1);
    const glm::fvec3 T = glm::normalize(M1 * ((t-t0)*(t-t0) * inv_h2) - M0 * ((t-t1)*(t-t1) * inv_h2) + D - C);
    glm::fvec3 N = M1 * ((t-t0) * inv_h) - M0 * ((t-t1) * inv_h);
    N -= glm::dot(T, N)*T;
    N = glm::normalize(N);
    const glm::fvec3 B = glm::normalize(glm::cross(T, N));

    return glm::fmat4(
        glm::fvec4(T, 0.0f),
        glm::fvec4(N, 0.0f),
        glm::fvec4(B, 0.0f),
        glm::fvec4(P, 1.0f)
    );
}

glm::fvec4 SmoothICurve::transform(float t, const glm::fvec4 &direction) const {
    glm::fmat4 M = getOrthonormalFrame(t);
    return M * direction;
}