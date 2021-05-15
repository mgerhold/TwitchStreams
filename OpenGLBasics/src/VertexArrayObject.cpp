//
// Created by coder2k on 15.05.2021.
//

#include <utility>
#include "VertexArrayObject.hpp"

VertexArrayObject::VertexArrayObject() noexcept {
    glGenVertexArrays(1U, &name);
}

VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) noexcept {
    std::swap(name, other.name);
}

VertexArrayObject &VertexArrayObject::operator=(VertexArrayObject &&other) noexcept {
    std::swap(name, other.name);
    return *this;
}

VertexArrayObject::~VertexArrayObject() {
    glDeleteVertexArrays(1U, &name);
}

void VertexArrayObject::bind() const noexcept {
    glBindVertexArray(name);
}

void VertexArrayObject::unbind() noexcept {
    glBindVertexArray(0U);
}
