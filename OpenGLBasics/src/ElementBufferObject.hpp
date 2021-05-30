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

    ~ElementBufferObject();

    [[nodiscard]] std::size_t count() const noexcept { return mNumIndices; }
    void submitData(const std::vector<GLuint>& indices, GLDataUsagePattern dataUsagePattern) noexcept;
    void bind() const noexcept;
    static void unbind() noexcept;

private:
    static GLuint sCurrentlyBoundName;
    GLuint mName{ 0U };
    std::size_t mNumIndices{ 0U };
};


