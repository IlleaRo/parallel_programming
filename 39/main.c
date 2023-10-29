#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uint128_tools/uint128_tools.h"
#include "finder_xyz/find_xyz.h"


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

    if (!find_xyz(N, &xyz)) {
        printf("X = %d\nY = %d\nZ = %d\n", xyz.x, xyz.y, xyz.z);
        exit(EXIT_SUCCESS);
    }
    else {
        printf("Something went wrong...\n");
        exit(EXIT_FAILURE);
    }
}