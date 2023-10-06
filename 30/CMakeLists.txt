cmake_minimum_required(VERSION 3.26)
project(30 C)

set(CMAKE_C_STANDARD 23)

find_package(OpenMP REQUIRED)

add_executable(splitting main.c)

# Add OpenMP flags to the compiler
if(OpenMP_C_FOUND)
    target_link_libraries(splitting OpenMP::OpenMP_C)
endif()