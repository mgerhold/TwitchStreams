//
// Created by coder2k on 22.05.2021.
//

#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer() noexcept {
    glGenVertexArrays(1U, &mVertexArrayObjectName);
    glGenBuffers(1U, &mVertexBufferObjectName);
    glGenBuffers(1U, &mElementBufferObjectName);
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(mVertexArrayObjectName, other.mVertexArrayObjectName);
    swap(mVertexBufferObjectName, other.mVertexBufferObjectName);
    swap(mElementBufferObjectName, other.mElementBufferObjectName);
    swap(mNumIndices, other.mNumIndices);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1U, &mVertexBufferObjectName);
    glDeleteVertexArrays(1U, &mVertexArrayObjectName);
    glDeleteBuffers(1U, &mElementBufferObjectName);
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    using std::swap;
    swap(mVertexArrayObjectName, other.mVertexArrayObjectName);
    swap(mVertexBufferObjectName, other.mVertexBufferObjectName);
    swap(mElementBufferObjectName, other.mElementBufferObjectName);
    swap(mNumIndices, other.mNumIndices);
    return *this;
}

void VertexBuffer::bind() const noexcept {
    bindVertexArrayObject();
    bindVertexBufferObject();
    bindElementBufferObject();
}

void VertexBuffer::unbind() noexcept {
    unbindElementBufferObject();
    unbindVertexBufferObject();
    unbindVertexArrayObject();
}

void VertexBuffer::bindVertexArrayObject() const noexcept {
    if (sCurrentlyBoundVertexArrayObjectName != mVertexArrayObjectName) {
        glBindVertexArray(mVertexArrayObjectName);
        sCurrentlyBoundVertexArrayObjectName = mVertexArrayObjectName;
    }
}

void VertexBuffer::bindVertexBufferObject() const noexcept {
    if (sCurrentlyBoundVertexBufferObjectName != mVertexBufferObjectName) {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectName);
        sCurrentlyBoundVertexBufferObjectName = mVertexBufferObjectName;
    }
}

void VertexBuffer::submitVertexData(const std::vector<GLfloat> &vertices,
                                    GLDataUsagePattern dataUsagePattern) const noexcept {
    bindVertexArrayObject();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(),
                 static_cast<GLenum>(dataUsagePattern));
}

void VertexBuffer::bindElementBufferObject() const noexcept {
    if (sCurrentlyBoundElementBufferObjectName != mElementBufferObjectName) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObjectName);
        sCurrentlyBoundElementBufferObjectName = mElementBufferObjectName;
    }
}

void VertexBuffer::unbindVertexArrayObject() noexcept {
    if (sCurrentlyBoundVertexArrayObjectName != 0U) {
        glBindBuffer(GL_ARRAY_BUFFER, 0U);
        sCurrentlyBoundVertexArrayObjectName = 0U;
    }
}

void VertexBuffer::unbindVertexBufferObject() noexcept {
    if (sCurrentlyBoundVertexBufferObjectName != 0U) {
        glBindVertexArray(0U);
        sCurrentlyBoundVertexBufferObjectName = 0U;
    }
}

void VertexBuffer::unbindElementBufferObject() noexcept {
    if (sCurrentlyBoundElementBufferObjectName != 0U) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
        sCurrentlyBoundElementBufferObjectName = 0U;
    }
}

void VertexBuffer::submitIndexData(const std::vector<GLuint> &indices,
                                   GLDataUsagePattern dataUsagePattern) noexcept {
    bindElementBufferObject();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gsl::narrow_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()),
                 indices.data(), static_cast<GLenum>(dataUsagePattern));
    mNumIndices = indices.size();
}
