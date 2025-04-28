#include <vector>
#include <variant>
#include <random>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>
#include "structures.h"

#pragma once

struct Distribution {

    std::minstd_rand g;

    Distribution() {};
    Distribution(int seed);

    virtual glm::vec3 sample(glm::vec3 point, glm::vec3 norm) = 0;
    virtual float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) = 0;
};

struct CosineDistribution : public Distribution {
    CosineDistribution() {};
    CosineDistribution(int seed);

    glm::vec3 sample(glm::vec3 point, glm::vec3 norm) override;
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) override;
};

struct LightDistribution : public Distribution {
    Object obj;

    LightDistribution() {};
    LightDistribution(int seed, Object obj);

    glm::vec3 sample(glm::vec3 point, glm::vec3 norm) override;
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) override;

    private:
    glm::vec3 box_sample(glm::vec3 point, glm::vec3 norm, glm::vec3 size);
    glm::vec3 ellips_sample(glm::vec3 point, glm::vec3 norm, glm::vec3 radius);
    glm::vec3 triangle_sample(glm::vec3 point, glm::vec3 a, glm::vec3 b, glm::vec3 c);
    float pdfBox();
    float pdfTriangle();
    float pdfEllips(glm::vec3 norm);
};

struct MixDistribution : public Distribution {
    std::vector<LightDistribution> lights;
    CosineDistribution cosine;

    MixDistribution() {};
    MixDistribution(CosineDistribution cosine, int seed);

    glm::vec3 sample(glm::vec3 point, glm::vec3 norm) override;
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) override;
    void add_light(LightDistribution light);
};