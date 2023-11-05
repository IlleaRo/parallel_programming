#include "finder_xyz/find_xyz.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>



int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    char *end_ptr;
    unsigned long long N = strtoul(argv[1], &end_ptr, 10);

    if (N == ULONG_MAX || N == 0) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    xyz_t xyz;
    int err_code;

    double start_time, end_time;

    start_time = omp_get_wtime();
    err_code = find_xyz(N, &xyz);
    end_time = omp_get_wtime();

    if (!err_code) {
        printf("X = %lu\nY = %lu\nZ = %lu\n", xyz.x, xyz.y, xyz.z);
        printf("Execution time: %0.2f sec\n", end_time - start_time);
        exit(EXIT_SUCCESS);
    }
    else {
        printf("Something went wrong...\nError code: %d\n", err_code);
        printf("Execution time: %0.2f sec\n", end_time - start_time);
        exit(EXIT_FAILURE);
    }
}