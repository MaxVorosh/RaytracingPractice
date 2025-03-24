#include <iostream>
#include "structures.h"

glm::vec3 Scene::generate_random_reflect(glm::vec3 norm) {
    std::normal_distribution<> dist(0, 1);
    float x = dist(g);
    float y = dist(g);
    float z = dist(g);
    glm::vec3 dir = glm::vec3(x, y, z);
    if (glm::dot(norm, dir) < 0) {
        dir *= -1;
    }
    return glm::normalize(dir);
}

float Scene::generate_random_uniform(float a, float b) {
    std::uniform_real_distribution<> dist(a, b);
    return dist(g);
}