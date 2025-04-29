cmake_minimum_required(VERSION 3.0)
project(raytracing)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "-fopenmp")

add_executable(raytracing main.cpp
	parser.cpp
    parser.h
    image_writer.cpp
    image_writer.h
    structures.h
    structures.cpp
    ray.cpp
    ray.h
    distribution.cpp
    distribution.h
    scene.h
    bvh.h
    bvh.cpp
)

target_include_directories(raytracing PUBLIC .)