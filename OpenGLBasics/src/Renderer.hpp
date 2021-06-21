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

class Renderer final {
public:
    struct VertexData {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
    };
    static_assert(alignof(VertexData) == 4);
    static_assert(sizeof(VertexData[2]) == 2 * sizeof(VertexData));
    static_assert(sizeof(VertexData) == 9 * sizeof(GLfloat));

    struct IndexData {
        GLuint i0, i1, i2;
    };
    static_assert(alignof(IndexData) == 4);
    static_assert(sizeof(IndexData) == 3 * sizeof(GLuint));
    static_assert(sizeof(IndexData[2]) == 2 * sizeof(IndexData));

public:
    Renderer();

    void beginScene() noexcept;
    void endScene() noexcept;
    void drawQuad(const glm::mat4& transform, const ShaderProgram& shader, const Texture& texture) noexcept;

private:
    static constexpr std::size_t maxVerticesPerBatch = 1000;
    std::vector<VertexData> mVertexData;
    std::vector<IndexData> mIndexData;
    VertexBuffer mVertexBuffer;
};
