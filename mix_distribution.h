#include "distribution.h"
#include "bvh.h"

struct MixDistribution : public Distribution {
    CosineDistribution cosine;
    BVH lights;

    MixDistribution() {};
    MixDistribution(CosineDistribution cosine, int seed);

    glm::vec3 sample(glm::vec3 point, glm::vec3 norm) override;
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) override;
};