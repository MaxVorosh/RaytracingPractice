#include <algorithm>
#include "bvh.h"

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
    auto suff = objects[r - l].aabb();
    for (int i = r - 1; i > l; --i) {
        // [l; i)
        float score_left = get_score(pref[i - l - 1]);
        float score_right = get_score(suff);
        float new_score = score_left * (i - l) + score_right * (r - i);
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

void BVH::build_node(std::vector<Object>& objects, int l, int r) {
    auto aabb = objects[l].aabb();
    std::vector<glm::vec3> p(4);
    for (int i = l + 1; i < r; ++i) {
        auto aabb2 = objects[i].aabb();
        p = {aabb.first, aabb.second, aabb2.first, aabb2.second};
        aabb = raw_aabb(p);
    }
    if (root == -1) {
        nodes.push_back(Node());
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
    float score = get_score(aabb);
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
    nodes.back().is_leaf = false;
    nodes.back().left = nodes.size();
    nodes.push_back(Node());
    build_node(objects, l, ind);
    nodes.back().right = nodes.size();
    nodes.push_back(Node());
    build_node(objects, ind, r);
}