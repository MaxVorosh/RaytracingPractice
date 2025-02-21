#include <vector>
#include <variant>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>

#pragma once

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color() = default;
    Color(unsigned char _r, unsigned char _g, unsigned char _b) {
        r = _r;
        g = _g;
        b = _b;
    }
};

struct ScenePixels {
    std::vector<Color> pixels;
    int width;
    int height;

    ScenePixels() = default;
    ScenePixels(int w, int h, std::vector<Color> p) {
        width = w;
        height = h;
        pixels = p;
    }
};

struct Plane {
    glm::vec3 normal;

    Plane() = default;
    Plane(glm::vec3 n) {
        normal = glm::normalize(n);
    }
};

struct Ellips {
    glm::vec3 radius;

    Ellips() = default;
    Ellips(glm::vec3 r) {
        radius = r;
    }
};

struct Box {
    glm::vec3 size;

    Box() = default;
    Box(glm::vec3 s) {
        size = s;
    }
};

using Shape = std::variant<Plane, Ellips, Box>;

struct Object {
    Shape shape;

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 color;
};

struct Scene {
    int width;
    int height;
    glm::vec3 bg_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float camera_fov_x;

    std::vector<Object> objects;

    Scene() = default;
};

struct Ray {
    glm::vec3 start;
    glm::vec3 direction;

    Ray() = default;
    Ray(glm::vec3 s, glm::vec3 d) {
        start = s;
        direction = d;
    }
};