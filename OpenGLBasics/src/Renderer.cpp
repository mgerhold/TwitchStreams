//
// Created by coder2k on 19.06.2021.
//

#include "Renderer.hpp"
#include "GLDataUsagePattern.hpp"
#include <gsl/gsl>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    mVertexData.reserve(maxTrianglesPerBatch * 3ULL);
    mIndexData.reserve(maxTrianglesPerBatch * 3ULL);
    mCurrentTextureNames.reserve(16);
    mVertexBuffer.setVertexAttributeLayout(
            VertexAttributeDefinition{ 3, GL_FLOAT, false }, VertexAttributeDefinition{ 4, GL_FLOAT, false },
            VertexAttributeDefinition{ 2, GL_FLOAT, false }, VertexAttributeDefinition{ 1, GL_UNSIGNED_INT, false });
}

void Renderer::beginFrame() noexcept {
    mVertexData.clear();
    mIndexData.clear();
    mRenderStats = RenderStats{};
}

void Renderer::endFrame() noexcept {
    flush();
}

void Renderer::drawQuad(const glm::vec3& translation,
                        const glm::vec3& rotationAxis,
                        float rotationAngle,
                        const glm::vec3& scale,
                        const ShaderProgram& shader,
                        const Texture& texture) noexcept {
    drawQuad(
            glm::scale(glm::rotate(glm::translate(glm::mat4{ 1.0f }, translation), rotationAngle, rotationAxis), scale),
            shader, texture);
}

template<typename T>
void Renderer::drawQuad(T&& transform, const ShaderProgram& shader, const Texture& texture) noexcept {
//void Renderer::drawQuad(const glm::mat4& transform, const ShaderProgram& shader, const Texture& texture) noexcept {
    const auto textureIterator = std::find(mCurrentTextureNames.cbegin(), mCurrentTextureNames.cend(), texture.mName);
    auto textureIndex = textureIterator == mCurrentTextureNames.cend()
                                ? -1
                                : static_cast<int>(std::distance(mCurrentTextureNames.cbegin(), textureIterator));
    if ((textureIndex < 0 && mCurrentTextureNames.size() == mCurrentTextureNames.capacity()) ||
        (mNumTrianglesInCurrentBatch + 2ULL > maxTrianglesPerBatch)) {
        flush();
    }
    if (textureIndex < 0) {
        mCurrentTextureNames.push_back(texture.mName);
        textureIndex = static_cast<int>(mCurrentTextureNames.size()) - 1;
    }
    const auto indexOffset = mVertexData.size();
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ -1.0f, -1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 0.0f, 0.0f },
                                      .texIndex{ gsl::narrow_cast<GLuint>(textureIndex) } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ 1.0f, -1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 1.0f, 0.0f },
                                      .texIndex{ gsl::narrow_cast<GLuint>(textureIndex) } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 1.0f, 1.0f },
                                      .texIndex{ gsl::narrow_cast<GLuint>(textureIndex) } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 0.0f, 1.0f },
                                      .texIndex{ gsl::narrow_cast<GLuint>(textureIndex) } });
    mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                    .i1 = gsl::narrow_cast<GLuint>(indexOffset + 1),
                                    .i2 = gsl::narrow_cast<GLuint>(indexOffset + 2) });
    mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                    .i1 = gsl::narrow_cast<GLuint>(indexOffset + 2),
                                    .i2 = gsl::narrow_cast<GLuint>(indexOffset + 3) });
    mNumTrianglesInCurrentBatch += 2ULL;
    mRenderStats.numVertices += 4ULL;
    mRenderStats.numTriangles += 2ULL;
}

void Renderer::flush() noexcept {
    if (mRenderStats.numBatches == 0ULL) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    if (mVertexData.empty()) {
        return;
    }
    mVertexBuffer.submitVertexData(mVertexData, GLDataUsagePattern::DynamicDraw);
    mVertexBuffer.submitIndexData(mIndexData, GLDataUsagePattern::DynamicDraw);
    mVertexBuffer.bind();
    for (std::size_t i = 0; i < mCurrentTextureNames.size(); ++i) {
        Texture::bind(mCurrentTextureNames[i], gsl::narrow_cast<GLint>(i));
    }
    glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLsizei>(mVertexBuffer.indicesCount()), GL_UNSIGNED_INT, nullptr);
    mVertexData.clear();
    mIndexData.clear();
    mCurrentTextureNames.clear();
    mNumTrianglesInCurrentBatch = 0ULL;
    mRenderStats.numBatches += 1ULL;
}
