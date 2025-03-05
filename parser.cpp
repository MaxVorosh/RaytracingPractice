#include "parser.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Scene parse(std::string filename) {
    std::ifstream fin(filename);
    Scene scene;
    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream sin(line);
        std::string command;
        sin >> command;
        if (command == "DIMENSIONS") {
            int width, height;
            sin >> width >> height;
            scene.width = width;
            scene.height = height;
        }
        else if (command == "BG_COLOR") {
            float r, g, b;
            sin >> r >> g >> b;
            scene.bg_color = glm::vec3(r, g, b);
        }
        else if (command == "CAMERA_POSITION") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.camera_position = glm::vec3(x, y, z);
        }
        else if (command == "CAMERA_RIGHT") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.camera_right = glm::normalize(glm::vec3(x, y, z));
        }
        else if (command == "CAMERA_UP") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.camera_up = glm::normalize(glm::vec3(x, y, z));
        }
        else if (command == "CAMERA_FORWARD") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.camera_forward = glm::normalize(glm::vec3(x, y, z));
        }
        else if (command == "CAMERA_FOV_X") {
            float fov;
            sin >> fov;
            scene.camera_fov_x = fov;
        }
        else if (command == "NEW_PRIMITIVE") {
            scene.objects.push_back(Object());
        }
        else if (command == "PLANE") {
            float nx, ny, nz;
            sin >> nx >> ny >> nz;
            scene.objects.back().shape = Plane(glm::vec3(nx, ny, nz));
        }
        else if (command == "ELLIPSOID") {
            float rx, ry, rz;
            sin >> rx >> ry >> rz;
            scene.objects.back().shape = Ellips(glm::vec3(rx, ry, rz));
        }
        else if (command == "BOX") {
            float sx, sy, sz;
            sin >> sx >> sy >> sz;
            scene.objects.back().shape = Box(glm::vec3(sx, sy, sz));
        }
        else if (command == "POSITION") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.objects.back().position = glm::vec3(x, y, z);
        }
        else if (command == "ROTATION") {
            float x, y, z, w;
            sin >> x >> y >> z >> w;
            scene.objects.back().rotation = glm::quat(w, x, y, z);
        }
        else if (command == "COLOR") {
            float r, g, b;
            sin >> r >> g >> b;
            scene.objects.back().color = glm::vec3(r, g, b);
        }
        else if (command == "METALLIC") {
            scene.objects.back().material = Material::Metallic;
        }
        else if (command == "DIELECTRIC") {
            scene.objects.back().material = Material::Dielectric;
        }
        else if (command == "IOR") {
            float ior;
            sin >> ior;
            scene.objects.back().ior = ior;
        }
        else if (command == "RAY_DEPTH") {
            sin >> scene.recursion_depth;
        }
        else if (command == "AMBIENT_LIGHT") {
            float r, g, b;
            sin >> r >> g >> b;
            scene.ambient_light = glm::vec3(r, g, b);
        }
        else if (command == "NEW_LIGHT") {
            scene.lights.push_back(Light());
        }
        else if (command == "LIGHT_INTENSITY") {
            float r, g, b;
            sin >> r >> g >> b;
            scene.lights.back().intensity = glm::vec3(r, g, b);
        }
        else if (command == "LIGHT_DIRECTION") {
            float x, y, z;
            sin >> x >> y >> z;
            scene.lights.back().config = DirectLightConfig(glm::vec3(x, y, z));
        }
        else if (command == "LIGHT_POSITION") {
            float x, y, z;
            sin >> x >> y >> z;
            PointLightConfig conf = std::get<PointLightConfig>(scene.lights.back().config);
            conf.position = glm::vec3(x, y, z);
            scene.lights.back().config = conf;
        }
        else if (command == "LIGHT_ATTENUATION") {
            float c0, c1, c2;
            sin >> c0 >> c1 >> c2;
            PointLightConfig conf = std::get<PointLightConfig>(scene.lights.back().config);
            conf.attenuation = glm::vec3(c0, c1, c2);
            scene.lights.back().config = conf;
        }
    }
    return scene;
}