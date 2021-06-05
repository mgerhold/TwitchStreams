//
// Created by coder2k on 15.05.2021.
//

#include "ShaderProgram.hpp"
#include "hash/hash.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <streambuf>
//#include <format>
#include <spdlog/fmt/fmt.h>

GLuint ShaderProgram::sCurrentlyBoundName{ 0U };

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mUniformLocations, other.mUniformLocations);
}

ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mUniformLocations, other.mUniformLocations);
    return *this;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(mName);
}

bool ShaderProgram::compile(std::string_view vertexShaderSource, std::string_view fragmentShaderSource) noexcept {
    if (hasBeenCompiled()) {
        glDeleteProgram(mName);
        mName = 0U;
    }
    GLuint vertexShaderName = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSourcesArray[] = { vertexShaderSource.data() };
    glShaderSource(vertexShaderName, 1U, vertexShaderSourcesArray, nullptr);
    glCompileShader(vertexShaderName);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShaderName, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShaderName, sizeof(infoLog), nullptr, infoLog);
        spdlog::error("Failed to compile vertex shader: {}", infoLog);
        glDeleteShader(vertexShaderName);
        return false;
    }

    GLuint fragmentShaderName = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSourcesArray[] = { fragmentShaderSource.data() };
    glShaderSource(fragmentShaderName, 1U, fragmentShaderSourcesArray, nullptr);
    glCompileShader(fragmentShaderName);
    glGetShaderiv(fragmentShaderName, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderName, sizeof(infoLog), nullptr, infoLog);
        spdlog::error("Failed to compile fragment shader: {}", infoLog);
        glDeleteShader(vertexShaderName);
        glDeleteShader(fragmentShaderName);
        return false;
    }

    this->mName = glCreateProgram();
    glAttachShader(this->mName, vertexShaderName);
    glAttachShader(this->mName, fragmentShaderName);
    glLinkProgram(this->mName);

    glGetProgramiv(this->mName, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->mName, sizeof(infoLog), nullptr, infoLog);
        spdlog::error("Failed to link shader program: {}", infoLog);
        glDeleteShader(vertexShaderName);
        glDeleteShader(fragmentShaderName);
        glDeleteProgram(this->mName);
        return false;
    }

    glDeleteShader(vertexShaderName);
    glDeleteShader(fragmentShaderName);
    cacheUniformLocations();
    spdlog::info("Successfully linked shader program.");
    return true;
}

void ShaderProgram::bind() const noexcept {
    if (sCurrentlyBoundName != mName) {
        glUseProgram(mName);
        sCurrentlyBoundName = mName;
    }
}

void ShaderProgram::unbind() noexcept {
    glUseProgram(0U);
    sCurrentlyBoundName = 0U;
}

tl::expected<ShaderProgram, std::string> ShaderProgram::generateFromFiles(const std::filesystem::path &vertexShaderPath,
                                                                          const std::filesystem::path &fragmentShaderPath) {
    using namespace std::literals::string_literals;
    if (!exists(vertexShaderPath)) {
        return tl::unexpected{ fmt::format("Vertex shader source file not found (filename was {}).",
                    vertexShaderPath.string()) };
        /*return tl::unexpected(std::format("Vertex shader source file not found (filename was {}).",
                                          vertexShaderPath.string()));*/
    }
    if (!exists(fragmentShaderPath)) {
        return tl::unexpected{ fmt::format("Fragment shader source file not found (filename was {}).",
                                          fragmentShaderPath.string()) };
        /*return tl::unexpected(std::format("Fragment shader source file not found (filename was {}).",
                                          fragmentShaderPath.string()));*/
    }
    const auto readStringFromFile = [](const std::filesystem::path& path) {
        std::ifstream vertexShaderFileStream{ path };
        return std::string{
                std::istreambuf_iterator<char>{vertexShaderFileStream},
                std::istreambuf_iterator<char>{}
        };
    };
    std::string vertexShaderSource = readStringFromFile(vertexShaderPath);
    std::string fragmentShaderSource = readStringFromFile(fragmentShaderPath);

    ShaderProgram shaderProgram;
    if (!shaderProgram.compile(vertexShaderSource, fragmentShaderSource)) {
        return tl::unexpected{ "Failed to create shader program."s };
    }
    return shaderProgram;
}

void ShaderProgram::setUniform(std::size_t uniformNameHash, const glm::mat4 &matrix) const noexcept {
    bind();
    const auto it = mUniformLocations.find(uniformNameHash);

    if (it == mUniformLocations.cend()) {
        //spdlog::error("Could not set uniform because the hash value {} could not be found.", uniformNameHash);
        spdlog::error("Could not set uniform \"{}\" since it could not be found.", Hash::getStringFromHash(uniformNameHash));
        return;
    }
    glUniformMatrix4fv(it->second, 1, false, glm::value_ptr(matrix));
}

void ShaderProgram::cacheUniformLocations() noexcept {
    GLint uniform_count = 0;
    glGetProgramiv(this->mName, GL_ACTIVE_UNIFORMS, &uniform_count);

    if (uniform_count != 0)
    {
        GLint 	max_name_len = 0;
        GLsizei length = 0;
        GLsizei count = 0;
        GLenum 	type = GL_NONE;
        glGetProgramiv(this->mName, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);

        auto uniform_name = std::make_unique<char[]>(max_name_len);

        for (GLint i = 0; i < uniform_count; ++i)
        {
            glGetActiveUniform(this->mName, i, max_name_len, &length, &count, &type, uniform_name.get());

            GLint location = glGetUniformLocation(this->mName, uniform_name.get());

            const std::size_t hash = Hash::hashString(std::string_view{
                    uniform_name.get(),
                    static_cast<std::size_t>(length) });
            mUniformLocations[hash] = location;
            mUniformNames[hash] = std::string{ uniform_name.get() };
            spdlog::info("uniform location for \"{}\" (hash: {:X}): {}", uniform_name.get(), hash, location);
        }
    }
}
