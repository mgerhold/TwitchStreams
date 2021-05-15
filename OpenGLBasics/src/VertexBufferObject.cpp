//
// Created by coder2k on 15.05.2021.
//

#include "VertexBufferObject.hpp"
#include <utility>

VertexBufferObject::VertexBufferObject() noexcept {
    glGenBuffers(1U, &name);
}

VertexBufferObject::VertexBufferObject(VertexBufferObject&& other) noexcept {
    std::swap(name, other.name);
}

VertexBufferObject &VertexBufferObject::operator=(VertexBufferObject&& other) noexcept {
    std::swap(name, other.name);
    return *this;
}

VertexBufferObject::~VertexBufferObject() {
    glDeleteBuffers(1U, &name);
    name = 0U;
}

void VertexBufferObject::bind() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, name);
}

void VertexBufferObject::unbind() noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, 0U);
}

void VertexBufferObject::submitData(const std::vector<GLfloat> &vertices,
                                    GLDataUsagePattern dataUsagePattern) const noexcept {
    bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(),
                 static_cast<GLenum>(dataUsagePattern));
}