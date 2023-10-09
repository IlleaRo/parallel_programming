#include "cpu/linear_solver_cpu.h"
#include "gpu/linear_solver_gpu.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {

    FILE* file;

    if ((file = fopen("task", "r")) == NULL)
    {
        fprintf(stderr, "Can't find \"task\"\n");
        exit(EXIT_FAILURE);
    }

    int dimension;

    if (fscanf(file, "%d", &dimension) == 0)
    {
        fprintf(stderr, "Incorrect data in file!\n");
        exit(EXIT_FAILURE);
    }

    double* les_l = (double*)malloc(sizeof(double) * dimension * dimension);
    double* les_r = (double*)malloc(sizeof(double) * dimension);
    if (!les_l)
    {
        fprintf(stderr, "Malloc error!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j) {
            if (fscanf(file, "%lf", les_l + i * dimension + j) == 0)
            {
                fprintf(stderr, "Incorrect data in file!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (!diagonal_dominance(les_l, dimension))
    {
        free(les_l);
        free(les_r);
        fprintf(stderr, "The method of simple iteration does not converge for the given values!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < dimension; ++i) {
        if (fscanf(file, "%lf", &les_r[i]) == 0)
        {
            fprintf(stderr, "Incorrect data in file!\n");
            exit(EXIT_FAILURE);
        }
    }

    double epsilon = 0.00001;
    if (fscanf(file, "%lf", &epsilon) == 0)
    {
        fprintf(stderr, "Incorrect data in file!\n");
        exit(EXIT_FAILURE);
    }
    fclose(file);


    double* result = (double*)calloc(dimension, sizeof(double));

    if (!result)
    {
        exit(EXIT_FAILURE);
    }

    clock_t start, end;
    double cpu_time_used;

    start = clock();

    switch(resolve_system_cpu(les_l, les_r, result, dimension, epsilon))
    {
        case 1:
        {
            free(les_l);
            free(les_r);
            free(result);
            fprintf(stderr, "Incorrect arguments for resolve_system_cpu!\n");
            exit(EXIT_FAILURE);
        }
        case 2:
        {
            free(les_l);
            free(les_r);
            free(result);
            fprintf(stderr, "Malloc/calloc error!\n");
            exit(EXIT_FAILURE);
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("CPU: %f sec\n", cpu_time_used);

    for (int i = 0; i < dimension; i++)
    {
        printf("x_%d = %0.10lf\n", i+1, result[i]);
    }


    for (int i = 0; i < dimension; ++i)
    {
        result[i] = 0;
    }

    cudaEvent_t start_c, end_c;
    float gpu_time;

    cudaEventCreate(&start_c);
    cudaEventCreate(&end_c);

    cudaEventRecord(start_c, 0);

    cudaError_t cudaStatus = resolve_system_gpu(les_l, les_r, result, dimension, epsilon);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        exit(EXIT_FAILURE);
    }
    cudaEventElapsedTime(&gpu_time, start_c, end_c);
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceReset failed!");
        exit(EXIT_FAILURE);
    }


    printf("GPU: %f sec\n", gpu_time);
    for (int i = 0; i < dimension; i++)
    {
        printf("x_%d = %0.10lf\n", i+1, result[i]);
    }

    cudaEventDestroy(start_c);
    cudaEventDestroy(end_c);

    free(les_l);
    free(les_r);
    free(result);

    exit(EXIT_SUCCESS);
}