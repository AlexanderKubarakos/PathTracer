#!/bin/bash

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ./CMakeLists.txt && make -j 6 && ./RayTracer $1
