#include "Mesh.hpp"

Mesh::Mesh( Mesh &&mesh ) noexcept
    : m_vaoID(mesh.m_vaoID)
    , m_vboID(mesh.m_vboID)
    , m_mode(GL_POINTS)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_vertices(std::move(mesh.m_vertices))
{
    mesh.m_vaoID = 0;
    mesh.m_vboID = 0;
    mesh.m_stride = 0;
    mesh.m_length = 0;
}


Mesh::Mesh( const Mesh &mesh )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(GL_POINTS)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_vertices(mesh.m_vertices.cbegin(), mesh.m_vertices.cend())
{}


Mesh::Mesh( const CSVFile &csv, const std::vector<std::pair<std::string, f32>> &columns, const GLenum mode )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
{
    std::vector<const std::vector<std::string> *> column_data;

    for (const auto &col : columns) {
        column_data.emplace_back(csv.getColumn(col.first));
    }

    m_stride = static_cast<u32>(column_data.size());
    m_length = csv.getRowCount();

    /*
     * Create packed vertex data like (x,y,z,t), (x,y,z,t), ...
     */
    for (u32 r = 0; r < m_length; r++) {
        for (u32 c = 0; c < m_stride; c++) {
            auto column = column_data[c];
            if (nullptr == column)
                m_vertices.push_back(columns[c].second); // Default value
            else
                m_vertices.push_back(std::stof(column->at(r)));
        }
    }
}


Mesh::Mesh( const CSVFile &csv,
            const std::pair<std::string, float> &T,
            const std::pair<std::string, float> &X,
            const std::pair<std::string, float> &Y,
            const std::pair<std::string, float> &Z,
            const Mesh *mesh, const GLenum mode )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
    , m_stride(4)
{
    const auto Xcol = csv.getColumn(X.first);
    const auto Ycol = csv.getColumn(Y.first);
    const auto Zcol = csv.getColumn(Z.first);
    const auto Tcol = csv.getColumn(T.first);
    m_length = csv.getRowCount();

    const float total_time = (nullptr == Tcol) ? (T.second * (m_length - 1)) : std::stof(Tcol->at(m_length - 1));
    const float inv_total_time = 1.0f / total_time;

    m_vertices.resize(4 * m_length);

    for (u32 r = 0; r < m_length; r++) {
        glm::fvec4 local_coords(1.0f);
        const float t = (nullptr == Tcol) ? (T.second * r) : std::stof(Tcol->at(r));
        local_coords.x = (nullptr == Xcol) ? X.second : std::stof(Xcol->at(r));
        local_coords.y = (nullptr == Ycol) ? Y.second : std::stof(Ycol->at(r));
        local_coords.z = (nullptr == Zcol) ? Z.second : std::stof(Zcol->at(r));

        glm::fvec3 coords = mesh->transform(t, local_coords);
        m_vertices[r * 4 + 0] = coords.x;
        m_vertices[r * 4 + 1] = coords.y;
        m_vertices[r * 4 + 2] = coords.z;
        m_vertices[r * 4 + 3] = t * inv_total_time;
    }
}


Mesh::Mesh( std::vector<float> &&positions, const u32 stride, const GLenum mode )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
    , m_stride(stride)
    , m_length(positions.size() / stride)
    , m_vertices(positions)
{}


void Mesh::push()
{
    if (m_vaoID || m_vboID)
        return;

    glCreateVertexArrays(1, &m_vaoID);
    glCreateBuffers(1, &m_vboID);

    glNamedBufferStorage(m_vboID, m_vertices.size() * sizeof(float), m_vertices.data(), 0);

    // First 3 floats are always position information
    glVertexArrayVertexBuffer(m_vaoID, 0, m_vboID, 0, m_stride * sizeof(float));
    glVertexArrayAttribFormat(m_vaoID, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(m_vaoID, 0);

    // Additional information
    for (u32 i = 1; i < m_stride - 2; i++) {
        glVertexArrayVertexBuffer(m_vaoID, i, m_vboID, (i + 2) * sizeof(float), m_stride * sizeof(float));
        glVertexArrayAttribFormat(m_vaoID, i, 1, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(m_vaoID, i);
    }
}

void Mesh::render() const
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(m_mode, 0, static_cast<GLsizei>(m_length));
}

glm::fmat4 Mesh::getOrthonormalFrame( f32 ) const
{
    return 1.0f; // identity
}

glm::fmat4 Mesh::getOrthonormalFrame( const glm::fvec2 & ) const
{
    return 1.0f; // identity
}

glm::fvec4 Mesh::transform( float t, const glm::fvec4 & ) const
{
    return glm::fvec4(0.0f);
}

glm::fvec4 Mesh::transform( const glm::fvec2 &local, const glm::fvec4 &direction ) const
{
    return glm::fvec4(0.0f);
}

glm::fvec3 Mesh::at( float t ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diffAt( float t ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diff2At( float t ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::at( const glm::fvec2 &local ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diffAt( const glm::fvec2 &local ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diff2At( const glm::fvec2 &local ) const
{
    return glm::fvec3(0.0f);
}
