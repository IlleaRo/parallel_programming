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
    for (int i = 1; i < n - 1; ++i) {
        for (int j = 1; j < k - 1; ++j) {
            for (int l = 1; l < m - 1; ++l) {
                area[i][j][l] = 0.167f * (
                        // height neighbours
                        area[i - 1][j][l] +
                        area[i + 1][j][l] +
                        // length neighbours
                        area[i][j - 1][l] +
                        area[i][j + 1][l] +
                        // width neighbours
                        area[i][j][l - 1] +
                        area[i][j][l + 1]
                );
            }
        }
    }

    //upper side n = 0
    for (int i = 1; i < k - 1; ++i) {
        for (int j = 1; j < m - 1; ++j) {
            if (i == k / 2 && j == m / 2) {
                continue;
            }
            area[0][i][j] = 0.2f * (
                    // height neighbours
                    area[1][i][j] +
                    // length neighbours
                    area[0][i - 1][j] +
                    area[0][i + 1][j] +
                    // width neighbours
                    area[0][i][j - 1] +
                    area[0][i][j + 1]
            );
        }
    }

    //front side m = 0
    for (int i = 1; i < n - 1; ++i) {
        for (int j = 1; j < k - 1; ++j) {
            area[i][j][0] = 0.2f * (
                    // height neighbours
                    area[i + 1][j][0] +
                    area[i - 1][j][0] +
                    // length neighbours
                    area[i][j - 1][0] +
                    area[i][j + 1][0] +
                    // width neighbours
                    area[i][j][1]
            );
        }
    }

    //back side m = m - 1
    for (int i = 1; i < n - 1; ++i) {
        for (int j = 1; j < k - 1; ++j) {
            area[i][j][m - 1] = 0.2f * (
                    // height neighbours
                    area[i + 1][j][m - 1] +
                    area[i - 1][j][m - 1] +
                    // length neighbours
                    area[i][j - 1][m - 1] +
                    area[i][j + 1][m - 1] +
                    // width neighbours
                    area[i][j][m - 2]
            );
        }
    }

    //left side k = 0
    for (int i = 1; i < n - 1; ++i) {
        for (int j = 1; j < m - 1; ++j) {
            area[i][0][j] = 0.2f * (
                    // height neighbours
                    area[i + 1][0][j] +
                    area[i - 1][0][j] +
                    // length neighbours
                    area[i][1][j] +
                    // width neighbours
                    area[i][0][j - 1] +
                    area[i][0][j + 1]
            );
        }
    }

    //right side k = k - 1
    for (int i = 1; i < n - 1; ++i) {
        for (int j = 1; j < m - 1; ++j) {
            area[i][k - 1][j] = 0.2f * (
                    // height neighbours
                    area[i + 1][k - 1][j] +
                    area[i - 1][k - 1][j] +
                    // length neighbours
                    area[i][k - 2][j] +
                    // width neighbours
                    area[i][k - 1][j - 1] +
                    area[i][k - 1][j + 1]
            );
        }
    }

    // edges
    for (int i = 1; i < n - 1; ++i) {
        area[i][0][0] = 0.25f * (
                // height neighbours
                area[i + 1][0][0] +
                area[i - 1][0][0] +
                // length neighbours
                area[i][1][0] +
                // width neighbours
                area[i][0][1]
                );
        area[i][0][m - 1] = 0.25f * (
                // height neighbours
                area[i + 1][0][0] +
                area[i - 1][0][0] +
                // length neighbours
                area[i][1][0] +
                // width neighbours
                area[i][0][m - 2]
        );
        area[i][k - 1][0] = 0.25f * (
                // height neighbours
                area[i + 1][k - 1][m - 1] +
                area[i - 1][k - 1][m - 1] +
                // length neighbours
                area[i][k - 2][m - 1] +
                // width neighbours
                area[i][k - 1][1]
        );
        area[i][k - 1][m - 1] = 0.25f * (
                // height neighbours
                area[i + 1][k - 1][m - 1] +
                area[i - 1][k - 1][m - 1] +
                // length neighbours
                area[i][k - 2][m - 1] +
                // width neighbours
                area[i][k - 1][m - 2]
        );
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
    unsigned long m = 5; // width
    unsigned long n = 4; // height

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

    calculate_temperature(area, k, m, n);

    print3DArray(area, k, m, n);

    exit(EXIT_SUCCESS);
}
