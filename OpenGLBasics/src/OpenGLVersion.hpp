//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "strong_type/strong_type.hpp"

using OpenGLMajorVersion = strong::type<int, struct OpenGLMajorVersion_>;
using OpenGLMinorVersion = strong::type<int, struct OpenGLMinorVersion_>;

struct OpenGLVersion {
    OpenGLMajorVersion major;
    OpenGLMinorVersion minor;
};