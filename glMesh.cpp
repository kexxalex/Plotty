#include "glMesh.hpp"

Mesh::Mesh(Mesh &&mesh)
    : m_vaoID(mesh.m_vaoID)
    , m_vboID(mesh.m_vboID)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_vertices(std::move(mesh.m_vertices))
{
    mesh.m_vaoID  = 0;
    mesh.m_vboID  = 0;
    mesh.m_stride = 0;
    mesh.m_length = 0;
}


Mesh::Mesh(const Mesh &mesh)
    : m_vaoID(0)
    , m_vboID(0)
    , m_stride(mesh.m_stride)
    , m_length(mesh.m_length)
    , m_vertices(mesh.m_vertices.cbegin(), mesh.m_vertices.cend())
{

}


Mesh::Mesh(const CSVFile &csv, const std::vector<std::pair<std::string, float>> &columns, GLenum mode)
    : m_vaoID(0)
    , m_vboID(0)
    , m_mode(mode)
{
    std::vector<const std::vector<std::string>*> column_data;
    
    for (const auto &col : columns)
    {
        column_data.emplace_back(csv.getColumn(col.first));
    }

    m_stride = uint32_t(column_data.size()) + 1;
    m_length = csv.getRowCount();

    const float inv_length = 1.0f / static_cast<float>(m_length);

    for (uint32_t r=0; r < m_length; r++)
    {
        for (uint32_t c=0; c < column_data.size(); c++)
        {
            const std::vector<std::string> * column = column_data[c];
            if (nullptr == column)
                m_vertices.push_back(columns[c].second);
            else
                m_vertices.push_back(std::stof(column->at(r)));
        }
        m_vertices.push_back(inv_length * static_cast<float>(r));
    }
}


void Mesh::push()
{
    if (m_vaoID || m_vboID)
        return;

    glCreateVertexArrays(1, &m_vaoID);
    glCreateBuffers(1, &m_vboID);

    glNamedBufferStorage(m_vboID, m_vertices.size()*sizeof(float), m_vertices.data(), 0);

    // First 3 floats are always position information
    glVertexArrayVertexBuffer(m_vaoID, 0, m_vboID, 0, m_stride*sizeof(float));
    glVertexArrayAttribFormat(m_vaoID, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(m_vaoID, 0);

    // Additional information
    for (uint32_t i=0; i < m_stride-3; i++)
    {
        glVertexArrayVertexBuffer(m_vaoID, i+1, m_vboID, (i+3) * sizeof(float), m_stride*sizeof(float));
        glVertexArrayAttribFormat(m_vaoID, i+1, 1, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(m_vaoID, i+1);
    }
}

void Mesh::render() const
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(m_mode, 0, static_cast<int>(m_length));
}
