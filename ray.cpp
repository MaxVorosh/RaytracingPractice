#include "ray.h"
#include <math.h>
#include <algorithm>
#include <iostream>

float aces_tone_map(float component) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    float color = (component * (a * component + b)) / (component * (c * component + d) + e);
    return glm::clamp(color, 0.f, 1.f);
}


int convert_color(float component) {
    component = aces_tone_map(component);
    component = pow(component, 1.0 / 2.2);
    return std::round(std::clamp(component * 255, 0.f, 255.f));
}

glm::vec3 get_color(Scene& scene, int obj_id) {
    return scene.objects[obj_id].color;
}

Ray generate_ray(Scene& scene, int x, int y) {
    float aspect_ratio = scene.width / float(scene.height);
    float tan_fov_x = std::tan(scene.camera_fov_x / 2.0);
    float tan_fov_y = tan_fov_x / aspect_ratio;
    float x_c = float(x) + 0.5;
    float y_c = float(y) + 0.5;
    float res_x = (2 * x_c / float(scene.width) - 1) * tan_fov_x;
    float res_y = -(2 * y_c / float(scene.height) - 1) * tan_fov_y;
    glm::vec3 dir = res_x * scene.camera_right + res_y * scene.camera_up + scene.camera_forward;
    return Ray(scene.camera_position, glm::normalize(dir));
}

std::pair<std::optional<float>, Color> intersection(Ray r, Scene& s) {
    std::optional<float> inter = std::nullopt;
    glm::vec3 col = s.bg_color;
    for (int i = 0; i < s.objects.size(); ++i) {
        std::optional<Intersection> res_int  = intersection(r, s.objects[i]);
        if (res_int.has_value() && (!inter.has_value() || inter.value() > res_int.value().t)) {
            inter = res_int.value().t;
            col = get_color(s, i);
        }
    }
    Color result_color = Color(convert_color(col.x), convert_color(col.y), convert_color(col.z));
    return {inter, result_color};
}

std::optional<Intersection> intersection(Ray r, Object obj) {
    r.start -= obj.position;
    glm::quat back_rotation = glm::inverse(obj.rotation);
    r.start = back_rotation * r.start;
    r.direction = glm::normalize(back_rotation * r.direction);
    std::optional<Intersection> res_int;
    std::visit([r, &res_int](const auto s){res_int = intersection(r, s);}, obj.shape);
    return res_int;
}

std::optional<Intersection> intersection(Ray r, Plane p) {
    float t = -(glm::dot(r.start, p.normal)) / (glm::dot(r.direction, p.normal));
    if (t < 0) {
        return std::nullopt;
    }

    //TODO think about other parameters
    return Intersection(t, p.normal, false);
}

std::optional<Intersection> intersection(Ray r, Ellips e) {
    glm::vec3 o_r = glm::vec3(r.start.x / e.radius.x, r.start.y / e.radius.y, r.start.z / e.radius.z);
    glm::vec3 d_r = glm::vec3(r.direction.x / e.radius.x, r.direction.y / e.radius.y, r.direction.z / e.radius.z);
    float c = glm::dot(o_r, o_r) - 1;
    float b2 = glm::dot(o_r, d_r);
    float a = glm::dot(d_r, d_r);
    float disc = b2 * b2 - a * c;
    if (disc < 0) {
        return std::nullopt;
    }
    float t1 = (-b2 - sqrt(disc)) / a;
    float t2 = (-b2 + sqrt(disc)) / a;
    if (t1 > t2) {
        std::swap(t1, t2);
    }
    if (t2 < 0) {
        return std::nullopt;
    }
    if (t1 < 0) {
        //TODO think about other parameters
        return Intersection(t2, glm::vec3(1.0), false);
    }
    //TODO think about other parameters
    return Intersection(t1, glm::vec3(1.0), false);
}

std::optional<Intersection> intersection(Ray r, Box b) {
    float t1x = (-b.size.x - r.start.x) / r.direction.x;
    float t2x = (b.size.x - r.start.x) / r.direction.x;
    float t1y = (-b.size.y - r.start.y) / r.direction.y;
    float t2y = (b.size.y - r.start.y) / r.direction.y;
    float t1z = (-b.size.z - r.start.z) / r.direction.z;
    float t2z = (b.size.z - r.start.z) / r.direction.z;
    if (t1x > t2x) {
        std::swap(t1x, t2x);
    }
    if (t1y > t2y) {
        std::swap(t1y, t2y);
    }
    if (t1z > t2z) {
        std::swap(t1z, t2z);
    }
    float t1 = std::max(t1x, std::max(t1y, t1z));
    float t2 = std::min(t2x, std::min(t2y, t2z));
    if (t1 > t2) {
        return std::nullopt;
    }
    if (t2 < 0) {
        return std::nullopt;
    }
    if (t1 < 0) {
        return Intersection(t2, glm::vec3(1.0), false);
    }
    return Intersection(t1, glm::vec3(1.0), false);
}