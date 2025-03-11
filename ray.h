#include "structures.h"
#include <optional>

#pragma once


Ray generate_ray(Scene& scene, int x, int y);
std::optional<Intersection> intersection(Ray r, Plane p);
std::optional<Intersection> intersection(Ray r, Ellips e);
std::optional<Intersection> intersection(Ray r, Box b);
std::optional<Intersection> intersection(Ray r, Object obj);
std::pair<std::optional<float>, Color> intersection(Ray r, Scene& s);

glm::vec3 get_color(Scene& scene, int obj_id, glm::vec3 start, Intersection inter);
