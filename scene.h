#include <vector>
#include <variant>
#include <random>
#include <glm/vec3.hpp>
#include "structures.h"
#include "distribution.h"
#include "mix_distribution.h"
#include "ray.h"
#include "bvh.h"

#pragma once

struct Scene {
    int width;
    int height;
    glm::vec3 bg_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float camera_fov_x;
    int recursion_depth;
    int samples;

    std::vector<Object> planes;
    BVH objects;
    BVH lights;

    MixDistribution dist;
    std::minstd_rand g = std::minstd_rand(239);

    Scene() = default;

    float generate_random_uniform(float a, float b) {
        std::uniform_real_distribution<> dist(a, b);
        return dist(g);
    }
};


Ray generate_ray(Scene& scene, int x, int y);
std::pair<std::optional<float>, glm::vec3> intersection(Ray r, Scene& s, int recursion_depth);
int convert_color(float component);

glm::vec3 get_color(Scene& scene, int obj_id, Ray objR, Intersection inter, int recursion_depth);