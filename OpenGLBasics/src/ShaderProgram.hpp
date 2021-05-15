//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include <glad/glad.h>
#include <string_view>

class ShaderProgram final {
public:
    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;

    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    virtual ~ShaderProgram();

    void compile(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) noexcept;
    void bind() const noexcept;
    static void unbind() noexcept;
    [[nodiscard]] bool hasBeenCompiled() const noexcept { return name != 0U; }

private:
    GLuint name{ 0U };
};


