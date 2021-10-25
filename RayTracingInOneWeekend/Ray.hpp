//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"
#include <cassert>

struct Ray {
    Ray(Point3 origin, Vec3 direction) : origin{ origin }, direction{ direction.normalized() } { }

    [[nodiscard]] Point3 evaluate(double t) const {
        assert(direction != Vec3{});
        return origin + t * direction;
    }

    const Point3 origin;
    const Vec3 direction;
};