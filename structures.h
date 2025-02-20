#include <vector>
#include <variant>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
};

struct Ellips {
    glm::vec3 radius;
};

struct Box {
    glm::vec3 size;
};

using Shape = std::variant<Plane, Ellips, Box>;

struct Object {
    Shape shape;

    glm::vec3 position;
    glm::vec4 rotation;
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

    Scene() = default;
};