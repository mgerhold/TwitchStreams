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
#include <chrono>

void Sandbox::setup() noexcept {
#ifdef DEBUG_BUILD
    spdlog::info("This is the debug build");
#else
    spdlog::info("This is the release build");
#endif
    for (const auto& directoryEntry :
         std::filesystem::directory_iterator(std::filesystem::current_path() / "assets" / "images")) {
        auto expectedTexture = Image::LoadFromFile(directoryEntry).and_then(Texture::Create);
        if (expectedTexture) {
            mTextures.push_back(std::move(expectedTexture.value()));
            spdlog::info("Loaded texture: {}", directoryEntry.path().string());
        } else {
            spdlog::warn("Could not load image: {} (Error: {})", directoryEntry.path().string(),
                         expectedTexture.error());
        }
    }

    setupShaders();

    const glm::vec4 v0{ -100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v1{ 100.0f, -100.0f, 0.0f, 1.0f };
    const glm::vec4 v2{ 100.0f, 100.0f, 0.0f, 1.0f };
    const glm::vec4 v3{ -100.0f, 100.0f, 0.0f, 1.0f };

    const std::vector<GLfloat> vertices{
        // position        // color             // UVs
        v0.x, v0.y, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// bottom left
        v1.x, v1.y, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom right
        v2.x, v2.y, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,// top right
        v3.x, v3.y, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f // top left
    };
    const std::vector<GLuint> indices{
        0, 1, 2, 2, 3, 0,
    };
    mShaderPrograms.front().bind();
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
    const auto projectionMatrix = glm::ortho<float>(
            gsl::narrow_cast<float>(-framebufferSize.width / 2), gsl::narrow_cast<float>(framebufferSize.width / 2),
            gsl::narrow_cast<float>(-framebufferSize.height / 2), gsl::narrow_cast<float>(framebufferSize.height / 2));
    for (auto& shaderProgram : mShaderPrograms) {
        shaderProgram.setUniform(Hash::staticHashString("projectionMatrix"), projectionMatrix);
    }

    mRenderer.beginFrame();
    const auto offset = glm::vec3{ -gsl::narrow_cast<float>(getFramebufferSize().width) / 2.0f + 20.0f,
                                   -gsl::narrow_cast<float>(getFramebufferSize().height) / 2.0f + 20.0f, 0.0f };
    constexpr int dimension = 100;
    for (int x = 0; x < dimension; ++x) {
        for (int y = 0; y < dimension; ++y) {
            mRenderer.drawQuad(offset + glm::vec3{ static_cast<float>(x) * 40.0f, y * 40.0f, 0.0f }, 0.0f,
                               glm::vec3{ 20.0f }, mShaderPrograms[y % mShaderPrograms.size()],
                               mTextures[x % mTextures.size()]);
        }
    }
    mRenderer.endFrame();
    const RenderStats& stats = mRenderer.stats();
    //spdlog::info("Stats: {} tris, {} vertices ({} batches)", stats.numTriangles, stats.numVertices, stats.numBatches);
}

void Sandbox::setupShaders() noexcept {
    auto expectedShaderProgram =
            ShaderProgram::generateFromFiles(std::filesystem::current_path() / "assets" / "shaders" / "default.vert",
                                             std::filesystem::current_path() / "assets" / "shaders" / "default.frag");
    if (!expectedShaderProgram) {
        spdlog::error("Failed to generate shader program from files: {}", expectedShaderProgram.error());
        return;
    }
    mShaderPrograms.push_back(std::move(expectedShaderProgram.value()));

    expectedShaderProgram =
            ShaderProgram::generateFromFiles(std::filesystem::current_path() / "assets" / "shaders" / "default.vert",
                                             std::filesystem::current_path() / "assets" / "shaders" / "debug.frag");
    if (!expectedShaderProgram) {
        spdlog::error("Failed to generate shader program from files: {}", expectedShaderProgram.error());
        return;
    }
    mShaderPrograms.push_back(std::move(expectedShaderProgram.value()));
}
