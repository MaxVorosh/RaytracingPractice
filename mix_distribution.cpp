#include "mix_distribution.h"

MixDistribution::MixDistribution(CosineDistribution cosine, int seed) : Distribution(seed) {
    this->cosine = cosine;
}

glm::vec3 MixDistribution::sample(glm::vec3 point, glm::vec3 norm) {
    std::uniform_int_distribution<> bin(0, 1);
    int group = bin(g);
    if (group == 0 || lights.objects.size() == 0) {
        return cosine.sample(point, norm);
    }
    
    std::uniform_int_distribution<> lightDist(0, lights.objects.size() - 1);
    int lightInd = lightDist(g);
    std::uniform_int_distribution<> addDist(0, 100000);
    int add = addDist(g);
    return LightDistribution(44 + lightInd + add, lights.objects[lightInd]).sample(point, norm);
}

float MixDistribution::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) {
    if (lights.objects.size() == 0) {
        return cosine.pdf(point, norm, d);
    }
    float p = 0.5 * cosine.pdf(point, norm, d);
    int N = lights.objects.size();
    float lights_p = lights.pdf(point, norm, d);
    p += 0.5 / float(N) * lights_p * 3.14;
    return p;
}