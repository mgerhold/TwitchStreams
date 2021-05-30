//
// Created by coder2k on 22.05.2021.
//

#include "VertexBuffer.hpp"

GLuint VertexBuffer::sCurrentlyBoundVertexArrayObjectName{ 0u };
GLuint VertexBuffer::sCurrentlyBoundVertexBufferObjectName{ 0u };

VertexBuffer::VertexBuffer() noexcept{
    glGenVertexArrays(1U, &mVertexArrayObjectName);
    glGenBuffers(1U, &mVertexBufferObjectName);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(mVertexArrayObjectName, other.mVertexArrayObjectName);
    swap(mVertexBufferObjectName, other.mVertexBufferObjectName);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1U, &mVertexBufferObjectName);
    glDeleteVertexArrays(1U, &mVertexArrayObjectName);
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(mVertexArrayObjectName, other.mVertexArrayObjectName);
    swap(mVertexBufferObjectName, other.mVertexBufferObjectName);
    return *this;
}

void VertexBuffer::bind() const noexcept {
    if (sCurrentlyBoundVertexArrayObjectName != mVertexArrayObjectName) {
        bindVertexArrayObject();
        sCurrentlyBoundVertexArrayObjectName = mVertexArrayObjectName;
    }
    if (sCurrentlyBoundVertexBufferObjectName != mVertexBufferObjectName) {
        bindVertexBufferObject();
        sCurrentlyBoundVertexBufferObjectName = mVertexBufferObjectName;
    }
}

void VertexBuffer::unbind() noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, 0U);
    glBindVertexArray(0U);
    sCurrentlyBoundVertexArrayObjectName = 0U;
    sCurrentlyBoundVertexBufferObjectName = 0U;
}

void VertexBuffer::bindVertexArrayObject() const noexcept {
    glBindVertexArray(mVertexArrayObjectName);
}

void VertexBuffer::bindVertexBufferObject() const noexcept {
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectName);
}

void VertexBuffer::submitData(const std::vector<GLfloat> &vertices,
                              GLDataUsagePattern dataUsagePattern) const noexcept {
    bindVertexArrayObject();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(),
                 static_cast<GLenum>(dataUsagePattern));
}
