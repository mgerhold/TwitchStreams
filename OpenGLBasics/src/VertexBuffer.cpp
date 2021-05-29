//
// Created by coder2k on 22.05.2021.
//

#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer() noexcept{
    glGenVertexArrays(1U, &vertexArrayObjectName);
    glGenBuffers(1U, &vertexBufferObjectName);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(vertexArrayObjectName, other.vertexArrayObjectName);
    swap(vertexBufferObjectName, other.vertexBufferObjectName);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1U, &vertexBufferObjectName);
    glDeleteVertexArrays(1U, &vertexArrayObjectName);
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(vertexArrayObjectName, other.vertexArrayObjectName);
    swap(vertexBufferObjectName, other.vertexBufferObjectName);
    return *this;
}

void VertexBuffer::bind() const noexcept {
    bindVertexArrayObject();
    bindVertexBufferObject();
}

void VertexBuffer::unbind() noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, 0U);
    glBindVertexArray(0U);
}

void VertexBuffer::bindVertexArrayObject() const noexcept {
    glBindVertexArray(vertexArrayObjectName);
}

void VertexBuffer::bindVertexBufferObject() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectName);
}

void VertexBuffer::submitData(const std::vector<GLfloat> &vertices,
                              GLDataUsagePattern dataUsagePattern) const noexcept {
    bindVertexArrayObject();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(),
                 static_cast<GLenum>(dataUsagePattern));
}
