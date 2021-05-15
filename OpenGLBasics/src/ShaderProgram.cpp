//
// Created by coder2k on 15.05.2021.
//

#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept {
    std::swap(name, other.name);
}

ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept {
    std::swap(name, other.name);
    return *this;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(name);
}

void ShaderProgram::compile(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) noexcept {
    if (hasBeenCompiled()) {
        glDeleteProgram(name);
        name = 0U;
    }
    // TODO: Check for errors
    GLuint vertexShaderName = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSourcesArray[] = { vertexShaderSource.data() };
    glShaderSource(vertexShaderName, 1U, vertexShaderSourcesArray, nullptr);
    glCompileShader(vertexShaderName);

    GLuint fragmentShaderName = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSourcesArray[] = { fragmentShaderSource.data() };
    glShaderSource(fragmentShaderName, 1U, fragmentShaderSourcesArray, nullptr);
    glCompileShader(fragmentShaderName);

    name = glCreateProgram();
    glAttachShader(name, vertexShaderName);
    glAttachShader(name, fragmentShaderName);
    glLinkProgram(name);

    glDeleteShader(vertexShaderName);
    glDeleteShader(fragmentShaderName);
}

void ShaderProgram::bind() const noexcept {
    glUseProgram(name);
}

void ShaderProgram::unbind() noexcept {
    glUseProgram(0U);
}
