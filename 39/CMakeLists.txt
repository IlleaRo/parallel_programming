cmake_minimum_required(VERSION 3.26)
project(finder C)

set(CMAKE_C_STANDARD 11)

add_executable(finder  main.c
        uint128_tools/uint128_tools.c
        finder_xyz/find_xyz.c
)

target_link_libraries(finder m)