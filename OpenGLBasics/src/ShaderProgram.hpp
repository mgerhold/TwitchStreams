//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "expected/expected.hpp"
#include <glad/glad.h>
#include <string_view>
#include <filesystem>
#include <unordered_map>
#include <glm/glm.hpp>

class ShaderProgram final {
public:
    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;

    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    ~ShaderProgram();

    [[nodiscard]] bool compile(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) noexcept;
    void bind() const noexcept;
    static void unbind() noexcept;
    [[nodiscard]] bool hasBeenCompiled() const noexcept {
        return mName != 0U;
    }
    static tl::expected<ShaderProgram, std::string> generateFromFiles(const std::filesystem::path& vertexShaderPath,
                                                                      const std::filesystem::path& fragmentShaderPath);
    void setUniform(std::size_t uniformNameHash, const glm::mat4& matrix) const noexcept;

private:
    void cacheUniformLocations() noexcept;

private:
    static GLuint sCurrentlyBoundName;
    GLuint mName{ 0U };
    std::unordered_map<std::size_t, GLint> mUniformLocations;
};
