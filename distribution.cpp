#include <iostream>
#include <variant>
#include <optional>
#include <glm/gtx/quaternion.hpp>
#include "structures.h"
#include "distribution.h"
#include "ray.h"


Distribution::Distribution(int seed) {
    g = std::minstd_rand(seed);
}

CosineDistribution::CosineDistribution(int seed) : Distribution(seed) {}

glm::vec3 CosineDistribution::sample(glm::vec3 point, glm::vec3 norm) {
    std::normal_distribution<> dist(0, 1);
    float x = dist(g);
    float y = dist(g);
    float z = dist(g);
    glm::vec3 sphere_point = glm::normalize(glm::vec3(x, y, z));
    return glm::normalize(sphere_point + norm);
}

float CosineDistribution::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) {
    return std::max(0.0, glm::dot(norm, d) / 3.14);
}

LightDistribution::LightDistribution(int seed, Object obj) : Distribution(seed) {
    this->obj = obj;
}

glm::vec3 LightDistribution::sample(glm::vec3 point, glm::vec3 norm) {
    
    if (Box* bval = std::get_if<Box>(&obj.shape)) {
        glm::vec3 size = bval->size;
        return box_sample(point, norm, size);
    }
    Ellips eval = std::get<Ellips>(obj.shape);
    glm::vec3 radius = eval.radius;
    return ellips_sample(point, norm, radius);
}

glm::vec3 LightDistribution::box_sample(glm::vec3 point, glm::vec3 norm, glm::vec3 size) {
    float w = size.x * size.y + size.x * size.z + size.y * size.z;
    std::uniform_real_distribution<> dist(0, w);
    float r = dist(g);

    std::uniform_real_distribution<> distBool(0, 1);
    int sign = 1;
    if (distBool(g) > 0.5) {
        sign = -1;
    }

    std::uniform_real_distribution<> distCoord(-1, 1);
    float x = size.x * distCoord(g);
    float y = size.y * distCoord(g);
    float z = size.z * distCoord(g);
    if (r < size.x * size.y) {
        z = size.z * sign;
    }
    else if (r < (size.x * size.y + size.x * size.z)) {
        y = size.y * sign;
    }
    else {
        x = size.x * sign;
    }
    glm::vec3 objPoint = glm::vec3(x, y, z);
    objPoint = obj.rotation * objPoint;
    objPoint += obj.position;
    return glm::normalize(objPoint - point);
}

glm::vec3 LightDistribution::ellips_sample(glm::vec3 point, glm::vec3 norm, glm::vec3 radius) {
    std::normal_distribution<> dist(0, 1);
    float x = dist(g);
    float y = dist(g);
    float z = dist(g);
    glm::vec3 sphere_point = glm::normalize(glm::vec3(x, y, z));
    glm::vec3 objPoint = sphere_point * radius;
    objPoint = obj.rotation * objPoint;
    objPoint += obj.position;
    return glm::normalize(objPoint - point);
}

float LightDistribution::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) {
    const float eps = 1e-4;
    Ray r = Ray(point + d * eps, d);
    std::optional<Intersection> raw_inter = intersection(r, obj);
    if (!raw_inter.has_value()) {
        return 0;
    }
    float p = 0;
    Intersection inter = raw_inter.value();
    bool isBox = false;
    if (Box* bval = std::get_if<Box>(&obj.shape)) {
        isBox = true;
    }
    Ray r2 = Ray(point + d * (inter.t + 2 * eps), d);
    std::optional<Intersection> raw_inter2 = intersection(r2, obj);
    float mult1 = inter.t * inter.t / std::abs(glm::dot(d, inter.norm));
    float mult2 = 0;
    if (raw_inter2.has_value()) {
        Intersection inter2 = raw_inter2.value();
        mult2 = (inter2.t * inter2.t) / std::abs(glm::dot(d, inter2.norm));
    }
    if (isBox) {
        p += pdfBox() * mult1;
        p += pdfBox() * mult2;
    }
    else {
        p += pdfEllips(inter.norm) * mult1;
        if (raw_inter2.has_value()) {
            Intersection inter2 = raw_inter2.value();
            p += pdfEllips(inter2.norm) * mult2;
        }
    }
    return p;
}

float LightDistribution::pdfBox() {
    Box bval = std::get<Box>(obj.shape);
    glm::vec3 size = bval.size;
    float w = 4 * (size.x * size.y + size.x * size.z + size.y * size.z);
    return 1 / w;
}

float LightDistribution::pdfEllips(glm::vec3 norm) {
    Ellips eval = std::get<Ellips>(obj.shape);
    glm::vec3 radius = eval.radius;
    glm::vec3 smth = glm::vec3(norm.x * radius.y * radius.z, radius.x * norm.y * radius.z, radius.x * radius.y * norm.z);
    return 1 / (4 * 3.14 * glm::length(smth));
}

MixDistribution::MixDistribution(CosineDistribution cosine, int seed) : Distribution(seed) {
    this->cosine = cosine;
}

glm::vec3 MixDistribution::sample(glm::vec3 point, glm::vec3 norm) {
    std::uniform_int_distribution<> bin(0, 1);
    int group = bin(g);
    if (group == 0 || lights.size() == 0) {
        return cosine.sample(point, norm);
    }
    
    std::uniform_int_distribution<> lightDist(0, lights.size() - 1);
    int lightInd = lightDist(g);
    return lights[lightInd].sample(point, norm);
}

float MixDistribution::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) {
    if (lights.size() == 0) {
        return cosine.pdf(point, norm, d);
    }
    float p = 0.5 * cosine.pdf(point, norm, d);
    int N = lights.size();
    for (int i = 0; i < N; ++i) {
        p += 0.5 / float(N) * lights[i].pdf(point, norm, d);
    }
    return p;
}

void MixDistribution::add_light(LightDistribution light) {
    lights.push_back(light);
}