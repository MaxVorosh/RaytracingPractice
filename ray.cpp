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

glm::vec3 get_color(Scene& scene, int obj_id, Ray objR, Intersection inter, int recursion_depth) {
    const float eps = 1e-4;
    glm::vec3 start = objR.start + objR.direction * inter.t;
    if (scene.objects[obj_id].material == Material::Diffuse) {
        glm::vec3 color = scene.ambient_light * scene.objects[obj_id].color;
        for (auto& light: scene.lights) {
            Ray r;
            glm::vec3 I;
            float dist = -1;
            if (const PointLightConfig* confVal = std::get_if<PointLightConfig>(&light.config)) {
                r = Ray(start, glm::normalize(confVal->position - start));
                dist = glm::distance(confVal->position, start);
                float att = confVal->attenuation.x + confVal->attenuation.y * dist + confVal->attenuation.z * std::pow(dist, 2);
                I = light.intensity / att;
            }
            else {
                DirectLightConfig conf = std::get<DirectLightConfig>(light.config);
                r = Ray(start, conf.direction);
                I = light.intensity;
            }
            r.start += r.direction * eps;
            if (!shadowIntersection(r, scene, dist)) {
                color += scene.objects[obj_id].color * std::max(glm::dot(inter.norm, r.direction), 0.f) * I;
            }
        }
        return color;
    }
    if (scene.objects[obj_id].material == Material::Metallic) {
        Ray r = Ray(start, objR.direction - 2.f * inter.norm * glm::dot(inter.norm, objR.direction));
        r.start += r.direction * eps;
        auto res = intersection(r, scene, recursion_depth + 1);
        return scene.objects[obj_id].color * res.second;
    }
    if (scene.objects[obj_id].material == Material::Dielectric) {
        Ray reflected = Ray(start, objR.direction - 2.f * inter.norm * glm::dot(inter.norm, objR.direction));
        reflected.start += reflected.direction * eps;
        glm::vec3 reflected_color = intersection(reflected, scene, recursion_depth + 1).second;

        float cosine1 = glm::dot(-objR.direction, inter.norm);
        float n1 = 1;
        float n2 = scene.objects[obj_id].ior;
        if (inter.is_inside) {
            std::swap(n1, n2);
        }
        float sine2 = n1 / n2 * sqrt(1 - pow(cosine1, 2));
        if (abs(sine2) > 1) {
            return reflected_color;
        }
        float cosine2 = sqrt(1 - pow(sine2, 2));
        Ray refracted = Ray(start, n1 / n2 * objR.direction + (n1 / n2 * cosine1 - cosine2) * inter.norm);
        refracted.start += refracted.direction * eps;
        glm::vec3 refracted_color = intersection(refracted, scene, recursion_depth + 1).second;

        float R0 = pow((n1 - n2) / (n1 + n2), 2);
        float R = R0 + (1 - R0) * pow(1 - cosine1, 5);
        if (inter.is_inside) {
            return R * reflected_color + (1 - R) * refracted_color;
        }
        return R * reflected_color + (1 - R) * refracted_color * scene.objects[obj_id].color;
    }
    return glm::vec3(0.0);
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

bool shadowIntersection(Ray r, Scene& s, float min_dist) {
    for (int i = 0; i < s.objects.size(); ++i) {
        std::optional<Intersection> res_int  = intersection(r, s.objects[i]);
        if (!res_int.has_value()) {
            continue;
        }
        if (min_dist == -1 || min_dist > res_int.value().t) {
            return true;
        }
    }
    return false;
}

std::pair<std::optional<float>, glm::vec3> intersection(Ray r, Scene& s, int recursion_depth) {
    std::optional<float> inter = std::nullopt;
    glm::vec3 col = s.bg_color;
    if (recursion_depth > s.recursion_depth) {
        return {inter, col};
    }
    for (int i = 0; i < s.objects.size(); ++i) {
        std::optional<Intersection> res_int  = intersection(r, s.objects[i]);
        if (res_int.has_value() && (!inter.has_value() || inter.value() > res_int.value().t)) {
            inter = res_int.value().t;
            col = get_color(s, i, r, res_int.value(), recursion_depth);
        }
    }
    return {inter, col};
}

std::optional<Intersection> intersection(Ray r, Object obj) {
    r.start -= obj.position;
    glm::quat back_rotation = glm::inverse(obj.rotation);
    r.start = back_rotation * r.start;
    r.direction = glm::normalize(back_rotation * r.direction);
    std::optional<Intersection> res_int;
    std::visit([r, &res_int](const auto s){res_int = intersection(r, s);}, obj.shape);
    if (!res_int.has_value()) {
        return res_int;
    }
    res_int.value().norm = glm::normalize(obj.rotation * res_int.value().norm);
    return res_int;
}

std::optional<Intersection> intersection(Ray r, Plane p) {
    float t = -(glm::dot(r.start, p.normal)) / (glm::dot(r.direction, p.normal));
    if (t < 0) {
        return std::nullopt;
    }
    glm::vec3 result_norm = p.normal;
    bool is_inside = false;
    if (glm::dot(r.direction, result_norm) > 0) {
        is_inside = true;
        result_norm *= -1;
    }
    return Intersection(t, result_norm, is_inside);
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
    float t = t1;
    if (t1 > t2) {
        std::swap(t1, t2);
    }
    if (t2 < 0) {
        return std::nullopt;
    }
    if (t1 < 0) {
        t = t2;
    }

    glm::vec3 point = r.start + r.direction * t;
    glm::vec3 norm = glm::vec3(
        point.x / (e.radius.x * e.radius.x), point.y / (e.radius.y * e.radius.y), point.z / (e.radius.z * e.radius.z)
    );
    norm = glm::normalize(norm);
    bool is_inside = false;
    if (glm::dot(norm, r.direction) > 0) {
        is_inside = true;
        norm *= -1;
    }

    return Intersection(t, norm, is_inside);
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
    float t = t1;
    if (t1 > t2) {
        return std::nullopt;
    }
    if (t2 < 0) {
        return std::nullopt;
    }
    if (t1 < 0) {
        t = t2;
    }
    glm::vec3 point = r.start + r.direction * t;
    glm::vec3 norm = point / b.size;
    const float eps = 1e-3;
    if (abs(norm.x) >= abs(norm.y) && abs(norm.x) >= abs(norm.z)) {
        norm.y = 0;
        norm.z = 0;
    }
    else if (abs(norm.y) >= abs(norm.z)) {
       norm.x = 0;
       norm.z = 0;
    }
    else {
        norm.x = 0;
        norm.y = 0;
    }
    norm = glm::normalize(norm);
    bool is_inside;
    if (glm::dot(r.direction, norm) > 0) {
        is_inside = true;
        norm *= -1;
    }
    return Intersection(t, norm, is_inside);
}