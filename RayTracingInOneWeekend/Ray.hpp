//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"

struct Ray {
    Point3 evaluate(double t) const {
        return origin + t * direction;
    }

    Point3 origin;
    Vec3 direction;
};