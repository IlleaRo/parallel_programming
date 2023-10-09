cmake_minimum_required(VERSION 3.25)
project(4 C)

set(CMAKE_C_STANDARD 23)


# Настройка для CUDA
find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})


add_executable(resolver main.c
        cpu/linear_solver_cpu.c
        gpu/linear_solver_gpu.cu)

target_link_libraries(resolver ${CUDA_LIBRARIES})
