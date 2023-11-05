#include "find_xyz.h"

#include <math.h>
#include <limits.h>

int is_int(double num) {
    return num == (int) num;
}


int is_square(unsigned long num) {
    long double sqrt_num = sqrt(num);

    if (is_int(sqrt_num)) {
        return sqrt_num * sqrt_num == num;
    }

    return 0;
}




 int find_xyz(unsigned long N, xyz_t* xyz) {
    if (!xyz) {
        return -1;
    }

    int count_squares = 0;
    int tmp;

    xyz->x = N + 1;
    for (xyz->x = N + 1; xyz->x < ULONG_MAX; xyz->x++) {
        for (xyz->y = N; xyz->y < xyz->x; xyz->y++) {
            tmp = 0;

            if (is_square(xyz->x + xyz->y)) {
                tmp++;
            }

            if (is_square(xyz->x - xyz->y)) {
                tmp++;
            }

            for (xyz->z = N; xyz->z < xyz->y; xyz->z++) {
                count_squares = tmp;

                if (is_square(xyz->x + xyz->z)) {
                    count_squares++;
                }

                if (is_square(xyz->x - xyz->z)) {
                    count_squares++;
                }

                if (is_square(xyz->y + xyz->z)) {
                    count_squares++;
                }

                if (count_squares > 4){
                    return 0;
                }

                if (is_square(xyz->y - xyz->z)) {
                    count_squares++;
                }

                if (count_squares > 4) {
                    return 0;
                }
            }
        }
    }
    return -1;
}