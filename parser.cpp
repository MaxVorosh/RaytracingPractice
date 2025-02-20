#include "parser.h"
#include <string>
#include <fstream>
#include <sstream>
#include <glm/gtx/quaternion.hpp>

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
    }
}