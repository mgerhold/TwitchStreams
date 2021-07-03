//
// Created by coder2k on 19.06.2021.
//

#include "Renderer.hpp"
#include "GLDataUsagePattern.hpp"
#include <gsl/gsl>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Renderer::Renderer() {
    mCommandBuffer.reserve(maxTrianglesPerBatch / 2ULL);
    mVertexData.reserve(maxTrianglesPerBatch * 3ULL);
    mIndexData.reserve(maxTrianglesPerBatch * 3ULL);
    mCurrentTextureNames.reserve(std::min(Texture::getTextureUnitCount(), 32));
    spdlog::info("GPU is capable of binding {} textures at a time.", mCurrentTextureNames.capacity());
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
    flushCommandBuffer();
    flushVertexAndIndexData();
}

void Renderer::drawQuad(const glm::vec3& translation,
                        float rotationAngle,
                        const glm::vec3& scale,
                        const ShaderProgram& shader,
                        const Texture& texture) noexcept {
    drawQuad(glm::scale(glm::rotate(glm::translate(glm::mat4{ 1.0f }, translation), rotationAngle,
                                    glm::vec3{ 0.0f, 0.0f, 1.0f }),
                        scale),
             shader, texture);
}

template<typename T>
void Renderer::drawQuad(T&& transform, const ShaderProgram& shader, const Texture& texture) noexcept {
    if (mCommandBuffer.size() < mCommandBuffer.capacity()) {
        mCommandBuffer.emplace_back(transform, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, shader.mName, texture.mName);
    } else {
        flushCommandBuffer();
    }
}

void Renderer::flushCommandBuffer() noexcept {
    if (mRenderStats.numBatches == 0ULL) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    if (mCommandBuffer.empty()) {
        return;
    }

    std::sort(mCommandBuffer.begin(), mCommandBuffer.end(), [](const RenderCommand& lhs, const RenderCommand& rhs) {
        // TODO: sort differently for transparent shaders
        return (lhs.shaderName < rhs.shaderName) ||
               (lhs.shaderName == rhs.shaderName && lhs.textureName < rhs.textureName);
    });
    auto currentStartIt = mCommandBuffer.begin();
    auto currentEndIt = std::upper_bound(
            mCommandBuffer.begin(), mCommandBuffer.end(), mCommandBuffer.front(),
            [](const RenderCommand& lhs, const RenderCommand& rhs) { return lhs.shaderName < rhs.shaderName; });

    while (currentStartIt != mCommandBuffer.end()) {// one iteration per shader
        mVertexData.clear();
        mIndexData.clear();
        mCurrentTextureNames.clear();
        ShaderProgram::bind(currentStartIt->shaderName);
        std::for_each(currentStartIt, currentEndIt, [&](const RenderCommand& renderCommand) {
            // TODO: use an indirection vector to optimize this as soon as there is a global asset manager
            int textureIndex = -1;
            for (std::size_t i = 0; i < mCurrentTextureNames.size(); ++i) {
                if (mCurrentTextureNames[i] == renderCommand.textureName) {
                    textureIndex = gsl::narrow_cast<int>(i);
                    break;
                }
            }

            if ((textureIndex < 0 && mCurrentTextureNames.size() == mCurrentTextureNames.capacity()) ||
                (mNumTrianglesInCurrentBatch + 2ULL > maxTrianglesPerBatch)) {
                flushVertexAndIndexData();
            }
            if (textureIndex < 0) {
                textureIndex = static_cast<int>(mCurrentTextureNames.size());
                mCurrentTextureNames.push_back(renderCommand.textureName);
            }

            // fill buffers
            const auto indexOffset = mVertexData.size();
            mVertexData.push_back(
                    VertexData{ .position = { renderCommand.transform * glm::vec4{ -1.0f, -1.0f, 0.0f, 1.0f } },
                                .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                                .texCoords = { 0.0f, 0.0f },
                                .texIndex = gsl::narrow_cast<GLuint>(textureIndex) });
            mVertexData.push_back(
                    VertexData{ .position = { renderCommand.transform * glm::vec4{ 1.0f, -1.0f, 0.0f, 1.0f } },
                                .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                                .texCoords = { 1.0f, 0.0f },
                                .texIndex = gsl::narrow_cast<GLuint>(textureIndex) });
            mVertexData.push_back(
                    VertexData{ .position = { renderCommand.transform * glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f } },
                                .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                                .texCoords = { 1.0f, 1.0f },
                                .texIndex = gsl::narrow_cast<GLuint>(textureIndex) });
            mVertexData.push_back(
                    VertexData{ .position = { renderCommand.transform * glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f } },
                                .color = { 1.0f, 1.0f, 1.0f, 1.0f },
                                .texCoords = { 0.0f, 1.0f },
                                .texIndex = gsl::narrow_cast<GLuint>(textureIndex) });
            mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                            .i1 = gsl::narrow_cast<GLuint>(indexOffset + 1),
                                            .i2 = gsl::narrow_cast<GLuint>(indexOffset + 2) });
            mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                            .i1 = gsl::narrow_cast<GLuint>(indexOffset + 2),
                                            .i2 = gsl::narrow_cast<GLuint>(indexOffset + 3) });
            mNumTrianglesInCurrentBatch += 2ULL;
            mRenderStats.numVertices += 4ULL;
            mRenderStats.numTriangles += 2ULL;
        });
        currentStartIt = currentEndIt;
        if (currentEndIt != mCommandBuffer.end()) {
            currentEndIt = std::upper_bound(
                    currentEndIt, mCommandBuffer.end(), *currentEndIt,
                    [](const RenderCommand& lhs, const RenderCommand& rhs) { return lhs.shaderName < rhs.shaderName; });
            flushVertexAndIndexData();
        }
    }
    mCommandBuffer.clear();
}
void Renderer::flushVertexAndIndexData() noexcept {
    if (mVertexData.empty()) {
        return;
    }
    // flush all buffers
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
