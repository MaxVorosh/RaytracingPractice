#include <iostream>
#include <string>
#include "parser.h"
#include "scene.h"
#include "image_writer.h"

void fill_scene(Scene& scene, ScenePixels& result_scene) {
    for (int i = 0; i < result_scene.width; ++i) {
        for (int j = 0; j < result_scene.height; ++j) {
            glm::vec3 result_color = glm::vec3(0.0);
            for (int k = 0; k < scene.samples; ++k) {
                Ray r = generate_ray(scene, i, j);
                auto inter = intersection(r, scene, 0);
                glm::vec3 col = inter.second;
                result_color += col;
            }
            result_color /= float(scene.samples);
            Color converted_color = Color(convert_color(result_color.x), convert_color(result_color.y), convert_color(result_color.z));
            result_scene.pixels[i + j * result_scene.width] = converted_color;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return -1;
    }
    std::string from_filename = argv[1];
    std::string to_filename = argv[2];
    Scene scene = parse(from_filename);
    ScenePixels result_scene = ScenePixels(scene.width, scene.height, std::vector<Color>(scene.width * scene.height));
    fill_scene(scene, result_scene);
    write_ppm_pixels(to_filename, result_scene);
    return 0;
}