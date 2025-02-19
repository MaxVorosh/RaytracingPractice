#include "image_writer.h"
#include <string>
#include <fstream>

void write_ppm_pixels(std::string filename, ScenePixels scene) {
    std::ofstream fout(filename, std::ios::binary);
    fout << "P6\n" << Scene.width << ' ' << Scene.height << "\n255\n";
    fout << scene.pixels.data();
    fout.close();
}