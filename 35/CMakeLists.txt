cmake_minimum_required(VERSION 3.26)
project(35 C)

set(CMAKE_C_STANDARD 11)

#find_package(MPI REQUIRED)
#include_directories(${MPI_INCLUDE_PATH})

add_executable(35
        main_mpi.c)

#target_link_libraries(35 ${MPI_LIBRARIES})