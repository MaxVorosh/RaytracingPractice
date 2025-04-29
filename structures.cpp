#include <stdexcept>
#include "structures.h"

std::pair<glm::vec3, glm::vec3> Object::aabb() {
    if (Plane* pval = std::get_if<Plane>(&shape)) {
        throw std::runtime_error("No aabb for plane");
    }
    std::pair<glm::vec3, glm::vec3> res;
    if (Ellips* eval = std::get_if<Ellips>(&shape)) {
        std::vector<glm::vec3> p = {-eval->radius, eval->radius};
        res = raw_aabb(p);
    }
    else if (Triangle* tval = std::get_if<Triangle>(&shape)) {
        std::vector<glm::vec3> p = {tval->a, tval->b, tval->c};
        res = raw_aabb(p);
    }
    else if (Box* bval = std::get_if<Box>(&shape)) {
        res = {-bval->size, bval->size};
    }
    else {
        throw std::runtime_error("Unexpected type");
    }
    std::vector<glm::vec3> points = {
        res.first, glm::vec3(res.first.x, res.first.y, res.second.z),
        glm::vec3(res.first.x, res.second.y, res.first.z), glm::vec3(res.first.x, res.second.y, res.second.z),
        glm::vec3(res.second.x, res.first.y, res.first.z), glm::vec3(res.second.x, res.first.y, res.second.z),
        glm::vec3(res.second.x, res.second.y, res.first.z), res.second
    };
    for (int i = 0; i < points.size(); ++i) {
        points[i] = rotation * points[i] + position;
    }
    return raw_aabb(points);
}

std::pair<glm::vec3, glm::vec3> Object::raw_aabb(std::vector<glm::vec3>& points) {
    glm::vec3 minim = points[0];
    glm::vec3 maxim = points[0];
    for (int i = 1; i < points.size(); ++i) {
        minim.x = std::min(minim.x, points[i].x);
        minim.y = std::min(minim.y, points[i].y);
        minim.z = std::min(minim.z, points[i].z);
        maxim.x = std::max(maxim.x, points[i].x);
        maxim.y = std::max(maxim.y, points[i].y);
        maxim.z = std::max(maxim.z, points[i].z);
    }
    return {minim, maxim};
}