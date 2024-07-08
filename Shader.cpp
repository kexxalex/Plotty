#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


bool loadShaderProgram( const std::string &filename, GLint shaderType, GLuint &shaderID )
{
    std::string sourceCode;
    std::ifstream shaderFile(filename);

    if (shaderFile) {
        std::stringstream sourceStringStream;
        sourceStringStream << shaderFile.rdbuf();

        shaderFile.close();

        sourceCode = sourceStringStream.str();
    }
    else {
        return false;
    }

    int success;
    char infoLog[1024];

    const char *source_cptr = sourceCode.c_str();

    shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &source_cptr, nullptr);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        std::cerr << "[ ERROR  ][Shader ] Error in: " << filename << std::endl << infoLog << std::endl;
        return false;
    }

    return true;
}


Shader::Shader( const std::string &shaderPath )
    : m_shaderName(shaderPath)
{
    Load();
}

Shader &Shader::operator=( Shader &&shader ) noexcept
{
    m_programID = shader.m_programID;
    m_uniforms = std::move(shader.m_uniforms);
    m_shaderName = std::move(shader.m_shaderName);
    return *this;
}

bool Shader::Reload()
{
    if (m_programID != 0) {
        glUseProgram(0);
        glDeleteProgram(m_programID);
    }

    m_uniforms.clear();
    return Load();
}

bool Shader::Load()
{
    std::cout << "[  INFO  ][Shader ] Create Shader: " << m_shaderName << '\n';

    GLuint vertexID(0), fragmentID(0), geometryID(0), computeID(0);
    bool hasVertex = loadShaderProgram(m_shaderName + ".vert", GL_VERTEX_SHADER, vertexID);
    bool hasFragment = loadShaderProgram(m_shaderName + ".frag", GL_FRAGMENT_SHADER, fragmentID);
    bool hasGeometry = loadShaderProgram(m_shaderName + ".geom", GL_GEOMETRY_SHADER, geometryID);
    bool hasCompute = loadShaderProgram(m_shaderName + ".comp", GL_COMPUTE_SHADER, computeID);

    // no shader source found
    if (!hasVertex && !hasFragment && !hasGeometry && !hasCompute)
        return false;

    // Create and Link the Shader Program, attach and delete available Shaders
    m_programID = glCreateProgram();

    if (hasVertex)
        glAttachShader(m_programID, vertexID);
    if (hasFragment)
        glAttachShader(m_programID, fragmentID);
    if (hasGeometry)
        glAttachShader(m_programID, geometryID);
    if (hasCompute)
        glAttachShader(m_programID, computeID);

    glLinkProgram(m_programID);

    if (hasVertex)
        glDeleteShader(vertexID);
    if (hasFragment)
        glDeleteShader(fragmentID);
    if (hasGeometry)
        glDeleteShader(geometryID);
    if (hasCompute)
        glDeleteShader(computeID);

    int success;
    char infoLog[1024];
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programID, 1024, nullptr, infoLog);
        std::cout << "[ ERROR  ][Shader ] Link:" << std::endl << infoLog << std::endl;
        return false;
    }

    return true;
}

void Shader::setBool( const std::string &name, const bool value )
{
    glProgramUniform1i(m_programID, getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt( const std::string &name, const i32 value )
{
    glProgramUniform1i(m_programID, getUniformLocation(name), value);
}

void Shader::setUInt( const std::string &name, const u32 value )
{
    glProgramUniform1ui(m_programID, getUniformLocation(name), value);
}

void Shader::setFloat( const std::string &name, const f32 value )
{
    glProgramUniform1f(m_programID, getUniformLocation(name), value);
}

void Shader::setDouble( const std::string &name, const f64 value )
{
    glProgramUniform1d(m_programID, getUniformLocation(name), value);
}

void Shader::setFloat2( const std::string &name, const glm::fvec2 &value )
{
    glProgramUniform2fv(m_programID, getUniformLocation(name), 1, &value.x);
}

void Shader::setFloat3( const std::string &name, const glm::fvec3 &value )
{
    glProgramUniform3fv(m_programID, getUniformLocation(name), 1, &value.x);
}

void Shader::setFloat4( const std::string &name, const glm::fvec4 &value )
{
    glProgramUniform4fv(m_programID, getUniformLocation(name), 1, &value.x);
}

void Shader::setDouble2( const std::string &name, const glm::dvec2 &value )
{
    glProgramUniform2dv(m_programID, getUniformLocation(name), 1, &value.x);
}

void Shader::setDouble3( const std::string &name, const glm::dvec3 &value )
{
    glProgramUniform3dv(m_programID, getUniformLocation(name), 1, &value.x);
}

void Shader::setMatrixFloat4( const std::string &name, const glm::fmat4 &matrix )
{
    glProgramUniformMatrix4fv(m_programID, getUniformLocation(name), 1, GL_FALSE, &matrix[0].x);
}

GLint Shader::getUniformLocation( const std::string &name )
{
    if (!m_uniforms.contains(name))
        m_uniforms[name] = glGetUniformLocation(m_programID, name.data());
    return m_uniforms[name];
}

Shader::~Shader()
{
    std::cout << "[  INFO  ][Shader ] Delete Shader: " << m_shaderName << '(' << m_programID << ')' << std::endl;

    glUseProgram(0);
    glDeleteProgram(m_programID);
    m_uniforms.clear();
}

void Shader::Bind() const
{
    glUseProgram(m_programID);
}
