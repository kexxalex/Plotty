#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/glm.hpp>

bool loadShaderProgram( const std::string &filename, GLint shaderType, GLuint &shaderID );


class Shader {
public:
    Shader() = default;

    explicit Shader( const std::string &shaderPath );

    Shader &operator=( Shader &&shader ) noexcept;

    ~Shader();

    void Bind() const;

    bool Reload();

    bool Load();

    constexpr unsigned int getID() const { return m_programID; }

    void setBool( const std::string &name, bool value );

    void setInt( const std::string &name, int value );

    void setUInt( const std::string &name, unsigned int value );

    void setFloat( const std::string &name, float value );

    void setFloat2( const std::string &name, const glm::fvec2 &value );

    void setFloat3( const std::string &name, const glm::fvec3 &value );

    void setFloat4( const std::string &name, const glm::fvec4 &value );

    void setDouble( const std::string &name, double value );

    void setDouble2( const std::string &name, const glm::dvec2 &value );

    void setDouble3( const std::string &name, const glm::dvec3 &value );

    void setMatrixFloat4( const std::string &name, const glm::fmat4 &matrix );

private:
    GLuint m_programID{ 0 };
    std::unordered_map<std::string, GLint> m_uniforms;
    std::string m_shaderName{ "-- This is no Shader --" };
};
