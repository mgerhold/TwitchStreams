//
// Created by coder2k on 15.05.2021.
//

#include "Sandbox.hpp"
#include "hash/hash.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <gsl/gsl>
#include <filesystem>
#include <string_view>

void Sandbox::setup() {
    setupShaders();

    const glm::vec4 v0{ -100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v1{ 100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v2{ 0.0f, 100.0f, 0.0f, 1.0f };

    const std::vector<GLfloat> vertices {
            // position         // color
            v0.x, v0.y,        1.0f, 0.0f, 0.0f, // top left 0
            v1.x, v1.y,        0.0f, 1.0f, 0.0f, // bottom left 1
            v2.x, v2.y,        0.0f, 0.0f, 1.0f, // bottom right 2
    };
    const std::vector<GLuint> indices {
            0, 1, 2,
    };
    mVertexBuffer.bind();
    mVertexBuffer.setVertexAttributeLayout(VertexAttributeDefinition{2, GL_FLOAT, false},
                                           VertexAttributeDefinition{3, GL_FLOAT, false});
    mVertexBuffer.submitData(vertices, GLDataUsagePattern::StaticDraw);
    mElementBufferObject.submitData(indices, GLDataUsagePattern::StaticDraw);
    mShaderProgram.bind();
}

void Sandbox::update() {
    processInput();
    render();
}

void Sandbox::processInput() noexcept {
    if (glfwGetKey(getGLFWWindowPointer(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(getGLFWWindowPointer(), true);
    }
}

void Sandbox::render() noexcept {
    const auto framebufferSize = getFramebufferSize();
    const auto projectionMatrix = glm::ortho<float>(gsl::narrow_cast<float>(-framebufferSize.width / 2),
                                                    gsl::narrow_cast<float>(framebufferSize.width / 2),
                                                    gsl::narrow_cast<float>(-framebufferSize.height / 2),
                                                    gsl::narrow_cast<float>(framebufferSize.height / 2));
    mShaderProgram.setUniform(hashString("projectionMatrix"), projectionMatrix);
    glClearColor(73.f / 255.f, 54.f / 255.f, 87.f / 255.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLsizei>(mElementBufferObject.count()),
            GL_UNSIGNED_INT, nullptr);
}

void Sandbox::setupShaders() noexcept {
    auto expectedShaderProgram = ShaderProgram::generateFromFiles(
            std::filesystem::current_path() / "assets" / "shaders" / "default.vert",
            std::filesystem::current_path() / "assets" / "shaders" / "default.frag");
    if (!expectedShaderProgram) {
        spdlog::error("Failed to generate shader program from files: {}", expectedShaderProgram.error());
        return;
    }
    mShaderProgram = std::move(expectedShaderProgram.value());
}
