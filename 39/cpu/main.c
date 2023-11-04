#include "uint128_tools/uint128_tools.h"
#include "finder_xyz/find_xyz.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <128 bit uint>");
        exit(EXIT_FAILURE);
    }

    __uint128_t N;
    N = read(argv[1], strlen(argv[1]) + 1);
    if (N == -1) {
        fprintf(stderr, "Incorrect arguments! Use ./splitting <128 bit uint>");
        exit(EXIT_FAILURE);
    }

    xyz_t xyz;
    int err_code;

    if (!(err_code = find_xyz(N, &xyz))) {
        printf("X = ");
        print_uint128(xyz.x);

        printf("\nY = ");
        print_uint128(xyz.y);

        printf("\nZ = ");
        print_uint128(xyz.z);

        printf("\n");

        exit(EXIT_SUCCESS);
    }
    else {
        printf("Something went wrong...\nError code: %d\n", err_code);
        exit(EXIT_FAILURE);
    }
}