//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include <glad/glad.h>

struct VertexAttributeDefinition {
    VertexAttributeDefinition(GLint count, GLenum type, GLboolean normalized) noexcept
        : count(count),
          type(type),
          normalized(normalized) { }

    GLint count;// e.g. 3 for a vec3 position vector
    GLenum type;// e.g. GL_FLOAT
    GLboolean normalized;
};