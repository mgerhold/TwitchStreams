//
// Created by coder2k on 15.05.2021.
//

#include "ElementBufferObject.hpp"
#include <gsl/gsl>

ElementBufferObject::ElementBufferObject() noexcept {
    glGenBuffers(1U, &name);
}

ElementBufferObject::ElementBufferObject(ElementBufferObject &&other) noexcept {
    std::swap(name, other.name);
    std::swap(numIndices, other.numIndices);
}

ElementBufferObject &ElementBufferObject::operator=(ElementBufferObject &&other) noexcept {
    std::swap(name, other.name);
    std::swap(numIndices, other.numIndices);
    return *this;
}

ElementBufferObject::~ElementBufferObject() {
    glDeleteBuffers(1U, &name);
}

void ElementBufferObject::submitData(const std::vector<GLuint> &indices, GLDataUsagePattern dataUsagePattern) noexcept {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gsl::narrow_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()),
            indices.data(), static_cast<GLenum>(dataUsagePattern));
    numIndices = indices.size();
}

void ElementBufferObject::bind() const noexcept {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, name);
}

void ElementBufferObject::unbind() noexcept {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
}
