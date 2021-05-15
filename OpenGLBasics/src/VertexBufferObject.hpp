//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include<glad/glad.h>
#include <vector>
#include "GLDataUsagePattern.hpp"

class VertexBufferObject final {
public:
    VertexBufferObject() noexcept; // default constructor
    VertexBufferObject(const VertexBufferObject&) = delete; // copy constructor
    VertexBufferObject(VertexBufferObject&& other) noexcept; // move constructor
    VertexBufferObject& operator=(const VertexBufferObject&) = delete; // copy assignment
    VertexBufferObject& operator=(VertexBufferObject&& other) noexcept; // move assignment
    virtual ~VertexBufferObject(); // destructor

    void bind() const noexcept;
    static void unbind() noexcept;
    void submitData(const std::vector<GLfloat>& vertices, GLDataUsagePattern dataUsagePattern) const noexcept;

private:
    GLuint name{ 0U };
};


