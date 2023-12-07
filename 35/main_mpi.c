#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>
#include <unistd.h>

void printArray(float *array, int k, int m, int n) {
    for (int l = 0; l < n; ++l) {
        printf("Layer %d:\n", l + 1);
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < m; ++j) {
                printf("%0.2f\t", array[l * (k * m) + i * m + j]);
            }
            printf("\n");
        }
    }
}

void calculate_temperature(float *area, int k, int m, int n, int n_by_rank) {
    int i = n_by_rank;
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < m; ++l) {
                if (i > 0 && j > 0 && l > 0 && i < n - 1 && j < k - 1 && l < m - 1) {
                    //typical
                    area[i * (k * m) + j * m + l] = 0.167f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + j * m + l] +
                            area[(i + 1) * (k * m) + j * m + l] +
                            // length neighbours
                            area[i * (k * m) + (j - 1) * m + l] +
                            area[i * (k * m) + (j + 1) * m + l] +
                            // width neighbours
                            area[i * (k * m) + j * m + l - 1] +
                            area[i * (k * m) + j * m + l + 1]
                    );
                } else if (i == 0 && j > 0 && l > 0 && j < k - 1 && l < m - 1) {
                    //upper side n = 0
                    if (j == k / 2 && l == m / 2) {
                        continue;
                    }
                    area[j * m + l] = 0.2f * (
                            // height neighbours
                            area[(k * m) + j * m + l] +
                            // length neighbours
                            area[(j - 1) * m + l] +
                            area[(j + 1) * m + l] +
                            // width neighbours
                            area[j * m + l - 1] +
                            area[j * m + l + 1]
                    );
                } else if (i > 0 && j > 0 && l == 0 && i < n - 1 && j < k - 1) {
                    //front side m = 0
                    area[i * (k * m) + j * m] = 0.2f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + j * m] +
                            area[(i + 1) * (k * m) + j * m] +
                            // length neighbours
                            area[i * (k * m) + (j - 1) * m] +
                            area[i * (k * m) + (j + 1) * m] +
                            // width neighbours
                            area[i * (k * m) + j * m + 1]
                    );
                } else if (i > 0 && j > 0 && i < n - 1 && j < k - 1 && l == m - 1) {
                    //back side m = m - 1
                    area[i * (k * m) + j * m + m - 1] = 0.2f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + j * m + m - 1] +
                            area[(i + 1) * (k * m) + j * m + m - 1] +
                            // length neighbours
                            area[i * (k * m) + (j - 1) * m + m - 1] +
                            area[i * (k * m) + (j + 1) * m + m - 1] +
                            // width neighbours
                            area[i * (k * m) + j * m + m - 2]
                    );
                } else if (i > 0 && j == 0 && l > 0 && i < n - 1 && l < m - 1) {
                    //left side k = 0
                    area[i * (k * m) + l] = 0.2f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + l] +
                            area[(i + 1) * (k * m) + l] +
                            // length neighbours
                            area[i * (k * m) + m + l] +
                            // width neighbours
                            area[i * (k * m) + l + 1] +
                            area[i * (k * m) + l - 1]
                    );
                } else if (i > 0 && j == k - 1 && l > 0 && i < n - 1 && l < m - 1) {
                    //right side k = k - 1
                    area[i * (k * m) + (k - 1) * m + l] = 0.2f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + (k - 1) * m + l] +
                            area[(i + 1) * (k * m) + (k - 1) * m + l] +
                            // length neighbours
                            area[i * (k * m) + (k - 2) * m + l] +
                            // width neighbours
                            area[i * (k * m) + (k - 1) * m + l - 1] +
                            area[i * (k * m) + (k - 1) * m + l + 1]
                    );
                } else if (i > 0 && j == 0 && l == 0 && i < n - 1) {
                    area[i * (k * m)] = 0.25f * (
                            // height neighbours
                            area[(i - 1) * (k * m)] +
                            area[(i + 1) * (k * m)] +
                            // length neighbours
                            area[i * (k * m) + m] +
                            // width neighbours
                            area[i * (k * m) + 1]
                    );
                } else if (i > 0 && j == 0 && l == m - 1 && i < n - 1) {
                    area[i * (k * m) + m - 1] = 0.25f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + m - 1] +
                            area[(i + 1) * (k * m) + m - 1] +
                            // length neighbours
                            area[i * (k * m) + m + m - 1] +
                            // width neighbours
                            area[i * (k * m) + m - 2]
                    );
                } else if (i > 0 && j == k - 1 && l == 0 && i < n - 1) {
                    area[i * (k * m) + (k - 1) * m] = 0.25f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + (k - 1) * m] +
                            area[(i + 1) * (k * m) + (k - 1) * m] +
                            // length neighbours
                            area[i * (k * m) + (k - 2) * m] +
                            // width neighbours
                            area[i * (k * m) + (k - 1) + 1]
                    );
                } else if (i > 0 && j == k - 1 && l == m - 1 && i < n - 1) {
                    area[i * (k * m) + (k - 1) * m + m - 1] = 0.25f * (
                            // height neighbours
                            area[(i - 1) * (k * m) + (k - 1) * m + m - 1] +
                            area[(i + 1) * (k * m) + (k - 1) * m + m - 1] +
                            // length neighbours
                            area[i * (k * m) + (k - 2) * m + m - 1] +
                            // width neighbours
                            area[i * (k * m) + (k - 1) * m + m - 2]
                    );
                }
            }
        }
}


void init_area(float *area,
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
                area[i * (k * m) + j * m + l] = T3;
            }
        }
    }

    // upper side
    area[k/2 * m + m/2] = T1; //upper side central
    for (int i = 0; i < k; ++i) {
        area[i * m] = T2;
        area[i * m + m - 1] = T2;
    }
    for (int i = 0; i < m; ++i) {
        area[i] = T2;
        area[(k - 1) * m + i] = T2;
    }

    // All from bottom side = 0!
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < m; ++j) {
            area[n * (k * m) + k * m + j] = 0;
        }
    }
}


int main(int argc, char* argv[]) {
    int k = 5; // length
    int m = 6; // width
    int n = 3; // height
    MPI_Init(NULL, NULL);

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

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    float *area = NULL;

    MPI_Win win;
    MPI_Win_allocate_shared((int)sizeof(float) * k * m * n, sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD, &area, &win);

    if (world_rank == 0) {
        init_area(area, k, m, n, T1, T2, T3);
        printArray(area, k, m, n);
    }

    int n_by_rank = world_size;
    if (world_size % n == 0) {
        world_size--;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < 5; ++i) {
        calculate_temperature(area, k, m, n, n_by_rank);
        n_by_rank = (n_by_rank + world_size) % n;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) {
        printf("\n");
        printArray(area, k, m, n);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_free(&win);
    exit(EXIT_SUCCESS);
}
