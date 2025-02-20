#include <iostream>
#include <string>
#include "parser.h"
#include "ray.h"
#include "image_writer.h"

void fill_scene(Scene& scene, ScenePixels& result_scene) {
    for (int i = 0; i < result_scene.width; ++i) {
        for (int j = 0; j < result_scene.height; ++j) {
            Ray r = generate_ray(scene, i, j);
            auto inter = intersection(r, scene);
            result_scene.pixels[i + j * result_scene.width] = inter.second;
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