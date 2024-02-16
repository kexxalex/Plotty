#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#ifdef __linux__
#include <GL/glew.h>
#include <glm/glm.hpp>
#elif _WIN32
#include "GL/glew.h"
#include "glm/glm.hpp"
#endif

bool loadShaderProgram(const std::string& filename, GLint shaderType, GLuint &shaderID);


class Shader {
public:
    Shader() = default;
    Shader(std::string_view shaderPath);

    Shader &operator=(Shader &&shader) {
        m_programID = shader.m_programID;
        m_uniforms = std::move(shader.m_uniforms);
        m_shaderName = std::move(shader.m_shaderName);
        return *this;
    }

    ~Shader();

    void Bind() const noexcept { glUseProgram(m_programID); }
    bool Reload() noexcept;

    bool Load() noexcept;

    constexpr unsigned int getID() const noexcept { return m_programID; }

    void setBool(std::string_view name, bool value) noexcept;
    void setInt(std::string_view name, int value) noexcept;
    void setUInt(std::string_view name, unsigned int value) noexcept;

    void setFloat(std::string_view name, float value) noexcept;
    void setFloat2(std::string_view name, const glm::fvec2 &value) noexcept;
    void setFloat3(std::string_view name, const glm::fvec3 &value) noexcept;
    void setFloat4(std::string_view name, const glm::fvec4 &value) noexcept;

    void setDouble(std::string_view name, double value) noexcept;
    void setDouble2(std::string_view name, const glm::dvec2 &value) noexcept;
    void setDouble3(std::string_view name, const glm::dvec3 &value) noexcept;

    void setMatrixFloat4(std::string_view name, const glm::fmat4 &matrix) noexcept;


private:
    GLuint m_programID{ 0 };
    std::unordered_map<std::string_view, GLint> m_uniforms;
    std::string m_shaderName{"-- This is no Shader --"};
};
