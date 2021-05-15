//
// Created by coder2k on 15.05.2021.
//

#pragma once

#include "strong_type/strong_type.hpp"

using WindowWidth = strong::type<int, struct WindowWidth_>;
using WindowHeight = strong::type<int, struct WindowHeight_>;

struct WindowSize {
    WindowWidth width;
    WindowHeight height;
};