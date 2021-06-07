//
// Created by coder2k on 15.05.2021.
//

#include "Sandbox.hpp"
#include "Image.hpp"
#include "Texture.hpp"
#include "hash/hash.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <string_view>

void Sandbox::setup() noexcept {
#ifdef DEBUG_BUILD
    spdlog::info("This is the debug build");
#else
    spdlog::info("This is the release build");
#endif
    spdlog::info("Current path is {}", std::filesystem::current_path().string());
    auto expectedTexture = Image::LoadFromFile(
                std::filesystem::current_path() / "assets" / "images" / "nojava.png").
                    and_then(Texture::Create);

    if (expectedTexture) {
        mTexture = std::move(expectedTexture.value());
        spdlog::info("Texture loaded (size {}x{})", mTexture.getWidth(), mTexture.getHeight());
    } else {
        spdlog::error("Failed to load texture: {}", expectedTexture.error());
    }

    setupShaders();

    const glm::vec4 v0{ -100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v1{ 100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v2{ 100.0f, 100.0f, 0.0f, 1.0f };
    const glm::vec4 v3{ -100.0f, 100.0f, 0.0f, 1.0f };

    const std::vector<GLfloat> vertices {
            // position        // color             // UVs
            v0.x, v0.y,        1.0f, 0.0f, 0.0f,    0.0f, 0.0f,     // bottom left
            v1.x, v1.y,        0.0f, 1.0f, 0.0f,    1.0f, 0.0f,     // bottom right
            v2.x, v2.y,        0.0f, 0.0f, 1.0f,    1.0f, 1.0f,     // top right
            v3.x, v3.y,        1.0f, 0.5f, 0.0f,    0.0f, 1.0f      // top left
    };
    const std::vector<GLuint> indices {
            0, 1, 2,
            2, 3, 0,
    };
    mVertexBuffer.bind();
    mVertexBuffer.setVertexAttributeLayout(VertexAttributeDefinition{2, GL_FLOAT, false},
                                           VertexAttributeDefinition{3, GL_FLOAT, false},
                                           VertexAttributeDefinition{2, GL_FLOAT, false});
    mVertexBuffer.submitVertexData(vertices, GLDataUsagePattern::StaticDraw);
    mVertexBuffer.submitIndexData(indices, GLDataUsagePattern::StaticDraw);
    mShaderProgram.bind();
    glClearColor(73.f / 255.f, 54.f / 255.f, 87.f / 255.f, 1.f);
}

void Sandbox::update() noexcept {
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
    mShaderProgram.setUniform(Hash::staticHashString("projectionMatrix"), projectionMatrix);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLsizei>(mVertexBuffer.indicesCount()),
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
