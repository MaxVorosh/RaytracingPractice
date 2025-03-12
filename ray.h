#include "structures.h"
#include <optional>

#pragma once


Ray generate_ray(Scene& scene, int x, int y);
std::optional<Intersection> intersection(Ray r, Plane p);
std::optional<Intersection> intersection(Ray r, Ellips e);
std::optional<Intersection> intersection(Ray r, Box b);
std::optional<Intersection> intersection(Ray r, Object obj);
std::pair<std::optional<float>, glm::vec3> intersection(Ray r, Scene& s, int recursion_depth);
int convert_color(float component);
bool shadowIntersection(Ray r, Scene& s, float min_dist);

glm::vec3 get_color(Scene& scene, int obj_id, Ray objR, Intersection inter, int recursion_depth);
