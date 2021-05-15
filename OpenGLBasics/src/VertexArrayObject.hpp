//
// Created by coder2k on 15.05.2021.
//

#pragma once


#include "VertexAttributeDefinition.hpp"
#include "GlUtils.hpp"
#include <gsl/gsl>
#include <glad/glad.h>
#include <concepts>

class VertexArrayObject final {
public:
    VertexArrayObject() noexcept;
    VertexArrayObject(const VertexArrayObject&) = delete;
    VertexArrayObject(VertexArrayObject&& other) noexcept;

    VertexArrayObject& operator=(const VertexArrayObject&) = delete;
    VertexArrayObject& operator=(VertexArrayObject&& other) noexcept;

    virtual ~VertexArrayObject();

    void bind() const noexcept;
    static void unbind() noexcept;

    void setVertexAttributes(std::convertible_to<VertexAttributeDefinition> auto... args) noexcept {
        bind();
        auto values = { args... };
        GLuint location {0U };
        std::uintptr_t offset { 0U };
        GLsizei stride { 0 };
        // calculate stride
        std::for_each(values.begin(), values.end(),
                        [&stride] (const VertexAttributeDefinition& definition) {
                            stride += gsl::narrow_cast<GLsizei>(GLUtils::getSizeOfGLType(definition.type)
                                    * definition.count);
                        });

        // set vertex attributes
        std::for_each(values.begin(), values.end(),
                        [&location, &offset, stride] (const VertexAttributeDefinition& definition) {
                            glVertexAttribPointer(location, definition.count, definition.type,
                                                  definition.normalized, stride, (void*)offset);
                            glEnableVertexAttribArray(location);
                            ++location;
                            offset += GLUtils::getSizeOfGLType(definition.type) * definition.count;
                        });
    }

private:
    GLuint name{ 0U };
};


