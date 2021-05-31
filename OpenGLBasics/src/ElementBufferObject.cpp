//
// Created by coder2k on 15.05.2021.
//

#include "ElementBufferObject.hpp"
#include <gsl/gsl>

ElementBufferObject::ElementBufferObject() noexcept {
    glGenBuffers(1U, &mName);
}

ElementBufferObject::ElementBufferObject(ElementBufferObject &&other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mNumIndices, other.mNumIndices);
}

ElementBufferObject &ElementBufferObject::operator=(ElementBufferObject &&other) noexcept {
    using std::swap;
    swap(mName, other.mName);
    swap(mNumIndices, other.mNumIndices);
    return *this;
}

ElementBufferObject::~ElementBufferObject() {
    glDeleteBuffers(1U, &mName);
}

void ElementBufferObject::submitData(const std::vector<GLuint> &indices, GLDataUsagePattern dataUsagePattern) noexcept {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gsl::narrow_cast<GLsizeiptr>(sizeof(GLuint) * indices.size()),
            indices.data(), static_cast<GLenum>(dataUsagePattern));
    mNumIndices = indices.size();
}

void ElementBufferObject::bind() const noexcept {
    if (sCurrentlyBoundName != mName) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mName);
        sCurrentlyBoundName = mName;
    }
}

void ElementBufferObject::unbind() noexcept {
    if (sCurrentlyBoundName != 0U) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
        sCurrentlyBoundName = 0U;
    }
}
