#include <vector>
#include <variant>
#include <random>
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

struct Triangle {
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;

    Triangle() = default;
    Triangle(glm::vec3 _x, glm::vec3 _y, glm::vec3 _z) {
        a = _x;
        b = _y;
        c = _z;
    }
};

using Shape = std::variant<Plane, Ellips, Box, Triangle>;

enum class Material {Diffuse, Metallic, Dielectric};

struct Object {
    Shape shape;

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 color;
    glm::vec3 emission = glm::vec3(0.0);
    Material material = Material::Diffuse;
    float ior;

    std::pair<glm::vec3, glm::vec3> aabb();

private:
    std::pair<glm::vec3, glm::vec3> raw_aabb(std::vector<glm::vec3>& points);
};