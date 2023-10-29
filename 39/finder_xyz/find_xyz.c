#include "find_xyz.h"

#include <math.h>

int is_int(long double num) {
    return num == (int) num;
}


int is_square(unsigned long long num) {
    long double sqrt_num = sqrtl(num);

    if (is_int(sqrt_num)) {
        return sqrt_num * sqrt_num == num;
    }

    return 0;
}




 int find_xyz(unsigned long long N, xyz_t* xyz) {
    if (!xyz) {
        return -1;
    }

    xyz->x = N + 1;
    while (1) {
        for (xyz->y = N; xyz->y < xyz->x; xyz->y++) {
            for (xyz->z = N; xyz->z < xyz->y; xyz->z++) {
                char count_squares = 0;

                if (is_square(xyz->x + xyz->y)) {
                    count_squares++;
                }

                if (is_square(xyz->x - xyz->y)) {
                    count_squares++;
                }

                if (is_square(xyz->x + xyz->z)) {
                    count_squares++;
                }

                if (is_square(xyz->x - xyz->z)) {
                    count_squares++;
                }

                if (is_square(xyz->y + xyz->z)) {
                    count_squares++;
                }

                if (count_squares == 5){
                    return 0;
                }

                if (is_square(xyz->y - xyz->z)) {
                    count_squares++;
                }

                if (count_squares >= 5) {
                    return 0;
                }
            }
        }
        xyz->x++;
    }
}