#include "structures.h"
#include <optional>

#pragma once


Ray generate_ray(Scene& scene, int x, int y);
std::optional<float> intersection(Ray r, Plane p);
std::optional<float> intersection(Ray r, Ellips e);
std::optional<float> intersection(Ray r, Box b);
std::optional<float> intersection(Ray r, Object obj);
std::pair<std::optional<float>, Color> intersection(Ray r, Scene& s);
