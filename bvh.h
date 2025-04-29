#include "structures.h"
#include "ray.h"

#pragma once

struct Node {
    glm::vec3 minim;
    glm::vec3 maxim;

    int left;
    int right;
    bool is_leaf;
};

struct BVH {
    std::vector<Node> nodes;
    int root;
    std::vector<Object> objects;

    BVH();
    void build_node(std::vector<Object>& objects);
    std::pair<std::optional<Intersection>, std::optional<Object>> intersect(Ray r);
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d);


private:
    void build_node(int l, int r);
    std::pair<std::optional<Intersection>, std::optional<Object>> intersect(Ray r, int node_id);
    float pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d, int node_id);
};


