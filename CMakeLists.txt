cmake_minimum_required(VERSION 3.19)
project(RayTracer)
add_compile_options(-O3 -Wall)
add_executable(RayTracer main.c PPM.c Ray.c Hitable.c Sphere.c Scene.c Camera.c Lambertian.c Metal.c Material.c Dielectric.c)