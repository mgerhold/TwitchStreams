//
// Created by coder2k on 19.06.2021.
//

#include "Renderer.hpp"
#include "GLDataUsagePattern.hpp"
#include <gsl/gsl>
#include <spdlog/spdlog.h>

Renderer::Renderer() {
    mVertexData.reserve(maxVerticesPerBatch);
    mVertexBuffer.setVertexAttributeLayout(VertexAttributeDefinition{ 3, GL_FLOAT, false },
                                           VertexAttributeDefinition{ 4, GL_FLOAT, false },
                                           VertexAttributeDefinition{ 2, GL_FLOAT, false });
}

void Renderer::beginScene() noexcept {
    mVertexData.clear();
    mIndexData.clear();
}

void Renderer::endScene() noexcept {
    if (mVertexData.empty()) {
        return;
    }
    mVertexBuffer.submitVertexData(mVertexData, GLDataUsagePattern::DynamicDraw);
    mVertexBuffer.submitIndexData(mIndexData, GLDataUsagePattern::DynamicDraw);
    mVertexBuffer.bind();
    glDrawElements(GL_TRIANGLES, gsl::narrow_cast<GLsizei>(mVertexBuffer.indicesCount()), GL_UNSIGNED_INT, nullptr);
}

void Renderer::drawQuad(const glm::mat4& transform, const ShaderProgram& shader, const Texture& texture) noexcept {
    const auto indexOffset = mVertexData.size();
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ -1.0f, -1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 0.0f, 0.0f } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ 1.0f, -1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 1.0f, 0.0f } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 1.0f, 1.0f } });
    mVertexData.push_back(VertexData{ .position = glm::vec3{ transform * glm::vec4{ -1.0f, 1.0f, 0.0f, 1.0f } },
                                      .color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f },
                                      .texCoords = glm::vec2{ 0.0f, 1.0f } });
    mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                    .i1 = gsl::narrow_cast<GLuint>(indexOffset + 1),
                                    .i2 = gsl::narrow_cast<GLuint>(indexOffset + 2) });
    mIndexData.push_back(IndexData{ .i0 = gsl::narrow_cast<GLuint>(indexOffset + 0),
                                    .i1 = gsl::narrow_cast<GLuint>(indexOffset + 2),
                                    .i2 = gsl::narrow_cast<GLuint>(indexOffset + 3) });
}