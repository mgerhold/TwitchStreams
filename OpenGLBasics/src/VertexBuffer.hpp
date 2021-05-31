//
// Created by coder2k on 22.05.2021.
//

#pragma once

#include "VertexAttributeDefinition.hpp"
#include "GLDataUsagePattern.hpp"
#include "GlUtils.hpp"
#include <glad/glad.h>
#include <gsl/gsl_util>
#include <concepts>
#include <vector>
#include <algorithm>

class VertexBuffer {
public:
    VertexBuffer() noexcept;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&& other) noexcept;
    ~VertexBuffer();

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    void bind() const noexcept;
    static void unbind() noexcept;

    [[nodiscard]] std::size_t indicesCount() const noexcept { return mNumIndices; }
    void setVertexAttributeLayout(std::convertible_to<VertexAttributeDefinition>auto... args) const;
    void submitVertexData(const std::vector<GLfloat>& vertices, GLDataUsagePattern dataUsagePattern) const noexcept;
    void submitIndexData(const std::vector<GLuint>& indices, GLDataUsagePattern dataUsagePattern) noexcept;

private:
    void bindVertexArrayObject() const noexcept;
    void bindVertexBufferObject() const noexcept;
    void bindElementBufferObject() const noexcept;
    static void unbindVertexArrayObject() noexcept;
    static void unbindVertexBufferObject() noexcept;
    static void unbindElementBufferObject() noexcept;

private:
    static inline GLuint sCurrentlyBoundVertexArrayObjectName{ 0U };
    static inline GLuint sCurrentlyBoundVertexBufferObjectName{ 0U };
    static inline GLuint sCurrentlyBoundElementBufferObjectName{ 0U };
    GLuint mVertexArrayObjectName{ 0u };
    GLuint mVertexBufferObjectName{ 0u };
    GLuint mElementBufferObjectName{ 0U };
    std::size_t mNumIndices{ 0U };
};

void VertexBuffer::setVertexAttributeLayout(std::convertible_to<VertexAttributeDefinition> auto... args) const {
    bind();
    auto values = { args... };
    GLuint location {0U };
    std::uintptr_t offset { 0U };
    GLsizei stride { 0 };
    // calculate stride
    std::for_each(values.begin(), values.end(),
                  [&stride] (const VertexAttributeDefinition& definition) {
                      stride += gsl::narrow_cast<GLsizei>(GLUtils::getSizeOfGLType(definition.type)
                                                          * definition.count);
                  });

    // set vertex attributes
    std::for_each(values.begin(), values.end(),
                  [&location, &offset, stride] (const VertexAttributeDefinition& definition) {
                      glVertexAttribPointer(location, definition.count, definition.type,
                                            definition.normalized, stride, (void*)offset);
                      glEnableVertexAttribArray(location);
                      ++location;
                      offset += GLUtils::getSizeOfGLType(definition.type) * definition.count;
                  });
}
