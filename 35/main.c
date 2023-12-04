#include <stdio.h>
#include <stdlib.h>

void init_area(float area[],
               unsigned long k,
               unsigned long m,
               unsigned long n,
               float T1,
               float T2,
               float T3)
{
    for (ssize_t i = 0; i < k; ++i) {
        for (ssize_t j = 0; j < m; ++j) {
            for (ssize_t l = 0; l < n; ++l) {
                area[i * (k * m) + j * m + l] = T3;
            }
        }
    }
}


int main(int argc, char* argv[]) {
    unsigned long k = 5; // length
    unsigned long m = 5; // height
    unsigned long n = 5; // width

    if (k < 1 || m < 1 || n < 1) {
        fprintf(stderr, "Incorrect dimensions of parallelepiped!\n");
        exit(EXIT_FAILURE);
    }

    if (k * m * n < k) {
        fprintf(stderr, "The parallelepiped too large!\n");
        exit(EXIT_FAILURE);
    }

    float T1 = 5.f;
    float T2 = 10.f;
    float T3 = 15.f;



    float *area = (float *)calloc(k * m * n, sizeof(float));

    init_area(area, k, m, n, T1, T2, T3);



    exit(EXIT_SUCCESS);
}
