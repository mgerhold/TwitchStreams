//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "GLDataUsagePattern.hpp"
#include <glad/glad.h>
#include <vector>

class ElementBufferObject final {
public:
    ElementBufferObject() noexcept;
    ElementBufferObject(const ElementBufferObject&) = delete;
    ElementBufferObject(ElementBufferObject&& other) noexcept;

    ElementBufferObject& operator=(const ElementBufferObject&) = delete;
    ElementBufferObject& operator=(ElementBufferObject&& other) noexcept;

    virtual ~ElementBufferObject();

    [[nodiscard]] std::size_t count() const noexcept { return numIndices; }
    void submitData(const std::vector<GLuint>& indices, GLDataUsagePattern dataUsagePattern) noexcept;
    void bind() const noexcept;
    static void unbind() noexcept;

private:
    GLuint name{ 0U };
    std::size_t numIndices{ 0U };
};


