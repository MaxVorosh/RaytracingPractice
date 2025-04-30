#include <algorithm>
#include <iostream>
#include "bvh.h"
#include "distribution.h"

float get_score(std::pair<glm::vec3, glm::vec3> aabb) {
    glm::vec3 size = aabb.second - aabb.first;
    return std::abs(size.x * size.y) + std::abs(size.x * size.z) + std::abs(size.y * size.z);
}

std::pair<int, float> find_partition(std::vector<Object>& objects, int l, int r, float score, bool (*sort_func)(Object, Object)) {
    std::sort(objects.begin() + l, objects.begin() + r, sort_func);
    int ind = -1;
    std::vector<std::pair<glm::vec3, glm::vec3>> pref(r - l);
    pref[0] = objects[l].aabb();
    std::vector<glm::vec3> points(4);
    for (int i = l + 1; i < r; ++i) {
        auto aabb = objects[i].aabb();
        points = {pref[i - l - 1].first, pref[i - l - 1].second, aabb.first, aabb.second};
        pref[i - l] = raw_aabb(points);
    }
    auto suff = objects[r - 1].aabb();
    for (int i = r - 1; i > l; --i) {
        // [l; i)
        float score_left = get_score(pref[i - l - 1]);
        float score_right = get_score(suff);
        float new_score = score_left * float(i - l) + score_right * float(r - i);
        if (new_score < score) {
            score = new_score;
            ind = i;
        }
        auto new_aabb = objects[i].aabb();
        points = {new_aabb.first, new_aabb.second, suff.first, suff.second};
        suff = raw_aabb(points);
    }
    return {ind, score};
}

BVH::BVH() {
    root = -1;
}

int BVH::depth(int node_id) {
    if (nodes[node_id].is_leaf) {
        return 1;
    }
    return 1 + std::max(depth(nodes[node_id].left), depth(nodes[node_id].right));
}

int BVH::nodes_cnt(int node_id) {
    if (nodes[node_id].is_leaf) {
       return nodes[node_id].right - nodes[node_id].left; 
    }
    return nodes_cnt(nodes[node_id].left) + nodes_cnt(nodes[node_id].right);
}

void BVH::build_node(std::vector<Object>& objects) {
    this->objects = objects;
    root = 0;
    nodes.push_back(Node());
    build_node(0, objects.size());
    // std::cout << objects.size() << std::endl;
    // std::cout << nodes.size() << std::endl;
    // std::cout << nodes[0].left << ' ' << nodes[0].right << std::endl;
    // std::cout << nodes[0].is_leaf << std::endl;
    // std::cout << depth(root) << std::endl;
    // std::cout << depth(nodes[root].left) << ' ' << depth(nodes[root].right) << std::endl;
    // std::cout << nodes_cnt(nodes[root].left) << ' ' << nodes_cnt(nodes[root].right) << std::endl;
}

void BVH::build_node(int l, int r) {
    auto aabb = objects[l].aabb();
    std::vector<glm::vec3> p(4);
    for (int i = l + 1; i < r; ++i) {
        auto aabb2 = objects[i].aabb();
        p = {aabb.first, aabb.second, aabb2.first, aabb2.second};
        aabb = raw_aabb(p);
    }
    nodes.back().minim = aabb.first;
    nodes.back().maxim = aabb.second;
    if (r - l <= 4) {
        nodes.back().is_leaf = true;
        nodes.back().left = l;
        nodes.back().right = r;
        return;
    }
    auto sort_x = [](Object a, Object b){
        auto aaabb = a.aabb();
        auto baabb = b.aabb();
        float a_center = aaabb.first.x + aaabb.second.x;
        float b_center = baabb.first.x + baabb.second.x;
        return a_center < b_center;
    };
    auto sort_y = [](Object a, Object b){
        auto aaabb = a.aabb();
        auto baabb = b.aabb();
        float a_center = aaabb.first.y + aaabb.second.y;
        float b_center = baabb.first.y + baabb.second.y;
        return a_center < b_center;
    };
    auto sort_z = [](Object a, Object b){
        auto aaabb = a.aabb();
        auto baabb = b.aabb();
        float a_center = aaabb.first.z + aaabb.second.z;
        float b_center = baabb.first.z + baabb.second.z;
        return a_center < b_center;
    };
    int best_axis = 0;
    float score = get_score(aabb) * float(r - l);
    auto res = find_partition(objects, l, r, score, sort_x);
    int ind = res.first;
    score = res.second;
    res = find_partition(objects, l, r, score, sort_y);
    if (res.first != -1) {
        ind = res.first;
        score = res.second;
        best_axis = 1;
    }
    res = find_partition(objects, l, r, score, sort_z);
    if (res.first != -1) {
        ind = res.first;
        best_axis = 2;
    }
    if (ind == -1) {
        nodes.back().is_leaf = true;
        nodes.back().left = l;
        nodes.back().right = r;
        return;
    }

    if (best_axis == 0) {
        std::sort(objects.begin() + l, objects.begin() + r, sort_x);
    }
    if (best_axis == 1) {
        std::sort(objects.begin() + l, objects.begin() + r, sort_y);
    }
    int node_ind = nodes.size() - 1;
    nodes.back().is_leaf = false;
    nodes.back().left = nodes.size();
    nodes.push_back(Node());
    build_node(l, ind);
    nodes[node_ind].right = nodes.size();
    nodes.push_back(Node());
    build_node(ind, r);
}

std::pair<std::optional<Intersection>, std::optional<Object>> BVH::intersect(Ray r) {
    return intersect(r, root);
}

std::pair<std::optional<Intersection>, std::optional<Object>> BVH::intersect(Ray r, int node_id) {
    Node node = nodes[node_id];
    if (node.is_leaf) {
        std::optional<float> inter = std::nullopt;
        std::optional<Object> obj = std::nullopt;
        std::optional<Intersection> full_inter = std::nullopt;
        for (int i = node.left; i < node.right; ++i) {
            std::optional<Intersection> res_int  = intersection(r, objects[i]);
            if (res_int.has_value() && (!inter.has_value() || inter.value() > res_int.value().t)) {
                inter = res_int.value().t;
                full_inter = res_int;
                obj = objects[i];
            }
        }
        return {full_inter, obj};
    }
    Node left_node = nodes[node.left];
    Node right_node = nodes[node.right];
    Object b_left = Object();
    glm::vec3 left_size = (left_node.maxim - left_node.minim) / glm::vec3(2.0);
    b_left.shape = Box(left_size);
    b_left.position = left_node.minim + left_size;
    Object b_right = Object();
    glm::vec3 right_size = (right_node.maxim - right_node.minim) / glm::vec3(2.0);
    b_right.shape = Box(right_size);
    b_right.position = right_node.minim + right_size;
    std::optional<Intersection> aabb_left_intersect = intersection(r, b_left);
    std::optional<Intersection> aabb_right_intersect = intersection(r, b_right);

    if (!aabb_left_intersect.has_value() && !aabb_right_intersect.has_value()) {
        return {std::nullopt, std::nullopt};
    }
    if (!aabb_left_intersect.has_value()) {
        return intersect(r, node.right);
    }
    if (!aabb_right_intersect.has_value()) {
        return intersect(r, node.left);
    }

    int first = node.left;
    int second = node.right;
    float f_t = aabb_left_intersect.value().t;
    float s_t = aabb_right_intersect.value().t;
    if (f_t > s_t) {
        std::swap(first, second);
        std::swap(f_t, s_t);
    }
    auto res = intersect(r, first);
    if (!res.first.has_value()) {
        return intersect(r, second);
    }
    if (res.first.value().t < s_t) {
        return res;
    }
    auto res2 = intersect(r, second);
    if (!res2.first.has_value() || res.first.value().t < res2.first.value().t) {
        return res;
    }
    return res2;
}

float BVH::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d) {
    return pdf(point, norm, d, root);
}

float BVH::pdf(glm::vec3 point, glm::vec3 norm, glm::vec3 d, int node_id) {
    Node node = nodes[node_id];
    if (node.is_leaf) {
        float res = 0;
        for (int i = node.left; i < node.right; ++i) {
            res += LightDistribution(0, objects[i]).pdf(point, norm, d); // Using that there is no sampling
        }
        return res;
    }
    Node left_node = nodes[node.left];
    Node right_node = nodes[node.right];
    Box b_left = Box((left_node.maxim - left_node.minim) / glm::vec3(2.0));
    Box b_right = Box((right_node.maxim - right_node.minim) / glm::vec3(2.0));
    std::optional<Intersection> aabb_left_intersect = intersection(Ray(point, d), b_left);
    std::optional<Intersection> aabb_right_intersect = intersection(Ray(point, d), b_right);

    float res = 0;
    if (aabb_left_intersect.has_value()) {
        res += pdf(point, norm, d, node.left);
    }
    if (aabb_right_intersect.has_value()) {
        res += pdf(point, norm, d, node.right);
    }
    return res;
}
