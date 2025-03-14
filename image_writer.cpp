#include "image_writer.h"
#include <string>
#include <fstream>
#include <iostream>

void write_ppm_pixels(std::string filename, ScenePixels scene) {
    std::ofstream fout(filename, std::ios::binary);
    fout << "P6\n" << scene.width << ' ' << scene.height << "\n255\n";
    fout.write(reinterpret_cast<char*>(scene.pixels.data()), scene.width * scene.height * 3);
    fout.close();
}