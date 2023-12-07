#include <stdio.h>
#include <stdlib.h>

void print3DArray(float ***array, unsigned long k, unsigned long m, unsigned long n) {
    for (int l = 0; l < n; ++l) {
        printf("Layer %d:\n", l + 1);
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < m; ++j) {
                printf("%0.2f\t", array[l][i][j]);
            }
            printf("\n");
        }
    }
}

void calculate_temperature(float ***area, unsigned long k, unsigned long m, unsigned long n)
{
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < m; ++l) {
                area[n][k][m] = 0.125f * (
                        // width neighbours
                        area[n - 1][k][m] +
                        area[n + 1][k][m] +
                        // length neighbours
                        area[n][k - 1][m] +
                        area[n][k + 1][m] +
                        // height neighbours
                        area[n][k][m - 1] + //
                        );
            }
        }
    }
}


float *** create_area(unsigned long k, unsigned long m, unsigned long n)
{
    float ***area = (float ***)malloc(n * sizeof(float**));
    if (area) {
        for (int i = 0; i < n; ++i) {
            area[i] = (float **)malloc(k * sizeof(float*));
            if (area[i])
            {
                for (int j = 0; j < k; ++j) {
                    area[i][j] = (float *) calloc(m, sizeof(float));
                    if (!area[i][j]) {
                        for (int l = 0; l < j-1; ++l) {
                            free(area[i][l]);
                        }
                        for (int p = 0; p < i; ++p) {
                            free(area[p]);
                        }
                        free(area);
                        return NULL;
                    }
                }
            }
            else {
                for (int p = 0; p < i - 1; ++p) {
                    free(area[p]);
                }
                free(area);
                return NULL;
            }
        }
    } else {
        return NULL;
    }
    return area;
}

void init_area(float ***area,
               unsigned long k, // length
               unsigned long m, // height
               unsigned long n, // width
               float T1,
               float T2,
               float T3)
{
    for (ssize_t i = 0; i < n - 1; ++i) {
        for (ssize_t j = 0; j < k; ++j) {
            for (ssize_t l = 0; l < m; ++l) {
                area[i][j][l] = T3;
            }
        }
    }

    // upper side
    area[0][k/2][m/2] = T1; //upper side central
    for (int i = 0; i < k; ++i) {
        area[0][i][0] = T2;
        area[0][i][m - 1] = T2;
    }
    for (int i = 0; i < m; ++i) {
        area[0][0][i] = T2;
        area[0][k - 1][i] = T2;
    }

    // All from bottom side = 0!
}


int main(int argc, char* argv[]) {
    unsigned long k = 5; // length
    unsigned long m = 5; // height
    unsigned long n = 3; // width

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


    float ***area = create_area(k, m, n);
    if (!area) {
        fprintf(stderr, "malloc err!\n");
        exit(EXIT_FAILURE);
    }

    init_area(area, k, m, n, T1, T2, T3);



    print3DArray(area, k, m, n);


    exit(EXIT_SUCCESS);
}
