//
// Created by coder2k on 19.06.2021.
//

#pragma once

#include "VertexBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct RenderStats {
    std::uint64_t numBatches{ 0ULL };
    std::uint64_t numTriangles{ 0ULL };
    std::uint64_t numVertices{ 0ULL };
};

class Renderer final {
public:
    struct VertexData {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        GLuint texIndex;
    };
    static_assert(alignof(VertexData) == 4);
    static_assert(sizeof(VertexData[2]) == 2 * sizeof(VertexData));
    static_assert(sizeof(VertexData) == 10 * sizeof(GLfloat));

    struct IndexData {
        GLuint i0, i1, i2;
    };
    static_assert(alignof(IndexData) == 4);
    static_assert(sizeof(IndexData) == 3 * sizeof(GLuint));
    static_assert(sizeof(IndexData[2]) == 2 * sizeof(IndexData));

public:
    Renderer();

    void beginFrame() noexcept;
    void endFrame() noexcept;
    void drawQuad(const glm::vec3& translation,
                  float rotationAngle,
                  const glm::vec3& scale,
                  const ShaderProgram& shader,
                  const Texture& texture) noexcept;
    template<typename T = glm::mat4>
    void drawQuad(T&& transform, const ShaderProgram& shader, const Texture& texture) noexcept;
    [[nodiscard]] const RenderStats& stats() const {
        return mRenderStats;
    }

private:
    struct RenderCommand {
        glm::mat4 transform;
        glm::vec4 color;
        GLuint shaderName;
        GLuint textureName;
    };

private:
    void flushCommandBuffer() noexcept;
    void flushVertexAndIndexData() noexcept;

private:
    static constexpr std::size_t maxTrianglesPerBatch = 10'000;
    std::uint64_t mNumTrianglesInCurrentBatch = 0ULL;
    std::vector<RenderCommand> mCommandBuffer;
    std::vector<VertexData> mVertexData;
    std::vector<IndexData> mIndexData;
    VertexBuffer mVertexBuffer;
    RenderStats mRenderStats;
    std::vector<GLuint> mCurrentTextureNames;
    GLuint mCurrentShaderProgramName{ 0U };
};
