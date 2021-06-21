//
// Created by coder2k on 15.05.2021.
//

#include "GlUtils.hpp"

namespace GLUtils {

    std::size_t getSizeOfGLType(GLenum dataType) {
        switch (dataType) {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
                return 1U;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_HALF_FLOAT:
                return 2U;
            case GL_INT:
            case GL_UNSIGNED_INT:
            case GL_FIXED:
            case GL_FLOAT:
                return 4U;
            case GL_DOUBLE:
                return 8U;
            default:
                return 0U;
        }
    }

}// namespace GLUtils