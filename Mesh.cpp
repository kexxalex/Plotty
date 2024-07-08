#include "Mesh.hpp"

Mesh::Mesh( Mesh &&mesh ) noexcept
    : m_vertices(std::move(mesh.m_vertices))
    , m_vaoID(mesh.m_vaoID)
    , m_vboID(mesh.m_vboID)
    , m_mode(GL_POINTS)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_bufferSize(0)
{
    mesh.m_vaoID = 0;
    mesh.m_vboID = 0;
    mesh.m_stride = 0;
    mesh.m_length = 0;
}


Mesh::Mesh( const Mesh &mesh )
    : m_vertices(mesh.m_vertices.cbegin(), mesh.m_vertices.cend())
    , m_vaoID(0)
    , m_vboID(0)
    , m_mode(GL_POINTS)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_bufferSize(0)
{}


Mesh::Mesh( const CSVFile &csv, const std::vector<std::pair<std::string, f32>> &columns, const GLenum mode )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
    , m_bufferSize(0)
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
            const auto column = column_data[c];
            if (nullptr == column)
                m_vertices.push_back(columns[c].second); // Default value
            else
                m_vertices.push_back(std::stof(column->at(r)));
        }
    }
}


Mesh::Mesh( const CSVFile &csv,
            const std::pair<std::string, f32> &T,
            const std::pair<std::string, f32> &X,
            const std::pair<std::string, f32> &Y,
            const std::pair<std::string, f32> &Z,
            const Mesh *mesh, const GLenum mode )
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
    , m_stride(4)
    , m_bufferSize(0)
{
    const auto Xcol = csv.getColumn(X.first);
    const auto Ycol = csv.getColumn(Y.first);
    const auto Zcol = csv.getColumn(Z.first);
    const auto Tcol = csv.getColumn(T.first);
    m_length = csv.getRowCount();

    const f32 total_time = (nullptr == Tcol) ? (T.second * (m_length - 1)) : std::stof(Tcol->at(m_length - 1));
    const f32 inv_total_time = 1.0f / total_time;

    m_vertices.resize(4 * m_length);

    for (u32 r = 0; r < m_length; r++) {
        glm::fvec4 local_coords(1.0f);
        const f32 t = (nullptr == Tcol) ? (T.second * r) : std::stof(Tcol->at(r));
        local_coords.x = (nullptr == Xcol) ? X.second : std::stof(Xcol->at(r));
        local_coords.y = (nullptr == Ycol) ? Y.second : std::stof(Ycol->at(r));
        local_coords.z = (nullptr == Zcol) ? Z.second : std::stof(Zcol->at(r));

        const glm::fvec3 coords = mesh->transform(t, local_coords);
        m_vertices[r * 4 + 0] = coords.x;
        m_vertices[r * 4 + 1] = coords.y;
        m_vertices[r * 4 + 2] = coords.z;
        m_vertices[r * 4 + 3] = t * inv_total_time;
    }
}


Mesh::Mesh( std::vector<f32> &&positions, const u32 stride, const GLenum mode )
    : m_vertices(positions)
    , m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
    , m_stride(stride)
    , m_length(positions.size() / stride)
    , m_bufferSize(0)
{}


void Mesh::push()
{
    if (m_vaoID > 0 && m_vboID > 0) {
        const auto newBufferSize = m_vertices.size() * sizeof(f32);
        if (m_bufferSize >= newBufferSize) {
            glNamedBufferSubData(m_vboID, 0, static_cast<GLsizeiptr>(newBufferSize), m_vertices.data());
            return;
        }
        glDeleteVertexArrays(1, &m_vaoID);
        glDeleteBuffers(1, &m_vboID);
    }

    glCreateVertexArrays(1, &m_vaoID);
    glCreateBuffers(1, &m_vboID);

    m_bufferSize = static_cast<GLsizeiptr>(m_vertices.size() * sizeof(f32));
    glNamedBufferStorage(m_vboID, m_bufferSize, m_vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // First 3 floats are always position information
    glVertexArrayVertexBuffer(m_vaoID, 0, m_vboID, 0, static_cast<GLsizei>(m_stride * sizeof(f32)));
    glVertexArrayAttribFormat(m_vaoID, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(m_vaoID, 0);

    // Additional information
    for (u32 i = 1; i < m_stride - 2; i++) {
        glVertexArrayVertexBuffer(m_vaoID, i, m_vboID, static_cast<GLintptr>((i + 2) * sizeof(f32)), static_cast<GLsizei>(m_stride * sizeof(f32)));
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

glm::fvec4 Mesh::transform( f32 t, const glm::fvec4 & ) const
{
    return glm::fvec4(0.0f);
}

glm::fvec4 Mesh::transform( const glm::fvec2 &local, const glm::fvec4 &direction ) const
{
    return glm::fvec4(0.0f);
}

glm::fvec3 Mesh::at( f32 t ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diffAt( f32 t ) const
{
    return glm::fvec3(0.0f);
}

glm::fvec3 Mesh::diff2At( f32 t ) const
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
