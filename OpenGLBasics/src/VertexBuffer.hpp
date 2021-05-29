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

    void setVertexAttributeLayout(std::convertible_to<VertexAttributeDefinition>auto... args) const;
    void submitData(const std::vector<GLfloat>& vertices, GLDataUsagePattern dataUsagePattern) const noexcept;

private:
    void bindVertexArrayObject() const noexcept;
    void bindVertexBufferObject() const noexcept;

private:
    GLuint mVertexArrayObjectName{ 0u };
    GLuint mVertexBufferObjectName{ 0u };
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
