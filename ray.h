#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <optional>
#include "structures.h"

#pragma once

struct Ray {
    glm::vec3 start;
    glm::vec3 direction;

    Ray() = default;
    Ray(glm::vec3 s, glm::vec3 d) {
        start = s;
        direction = d;
    }
};

struct Intersection {
    float t;
    glm::vec3 norm;
    bool is_inside;

    Intersection(float _t, glm::vec3 n, bool inside) {
        t = _t;
        norm = n;
        is_inside = inside;
    }
};

std::optional<Intersection> intersection(Ray r, Plane p);
std::optional<Intersection> intersection(Ray r, Ellips e);
std::optional<Intersection> intersection(Ray r, Box b);
std::optional<Intersection> intersection(Ray r, Triangle t);
std::optional<Intersection> intersection(Ray r, Object obj);