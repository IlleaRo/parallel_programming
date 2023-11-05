cmake_minimum_required(VERSION 3.26)
project(finder C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)


add_executable(finder  main.c
        finder_xyz/find_xyz.c
)

target_link_libraries(finder PRIVATE OpenMP::OpenMP_C)
target_link_libraries(finder PRIVATE m)