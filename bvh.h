#include "structures.h"

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

    BVH();
    void build_node(std::vector<Object>& objects);

private:
    void build_node(std::vector<Object>& objects, int l, int r);
};


