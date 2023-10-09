#include "linear_solver_gpu.h"

__global__ void create_b(const double *, const double *, double *);
__global__ void create_a(const double *, double *, int);
__global__ void resolve_row(const double *, const double *, double *, double *, double *, double, int);

cudaError_t resolve_system_gpu(const double* left, const double* right, double* result, int dimension, double epsilon)
{
    double* dev_left = 0;
    double* dev_right = 0;
    double* dev_res = 0;
    double* dev_b_column = 0;
    double* dev_a_matrix = 0;
    double* dev_prev_step = 0;
    double* dev_tmp = 0;

    cudaError_t cudaStatus;
    cudaStatus = cudaSetDevice(0);


    if (cudaStatus != cudaSuccess)
    {
        return cudaStatus;
    }

    // Allocate GPU buffers
    cudaStatus = cudaMalloc((void**)&dev_left, dimension * dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&dev_right, dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&dev_res, dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        return cudaStatus;
    }


    cudaStatus = cudaMalloc((void**)&dev_b_column, dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&dev_a_matrix, dimension * dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        return cudaStatus;
    }


    cudaStatus = cudaMalloc((void**)&dev_prev_step, dimension * sizeof(double));
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        return cudaStatus;
    }

    cudaStatus = cudaMalloc((void**)&dev_tmp, dimension * sizeof(double));
    if (cudaStatus != cudaSuccess) {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    // Copy input matrixes from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_left, left, dimension * dimension * sizeof(double), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaMemcpy(dev_right, right, dimension * sizeof(double), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaMemcpy(dev_prev_step, result, dimension * sizeof(double), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    // Start with creation of b_column
    create_b <<<1, dimension>>> (dev_left, dev_right, dev_b_column);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }


    dim3 threadsPerBlock(dimension, dimension);
    dim3 blocksPerGrid(1, 1);
    if (dimension * dimension > 512)
    {
        threadsPerBlock.x = 512;
        threadsPerBlock.y = 512;
        blocksPerGrid.x = (unsigned int)(double(dimension) / double(threadsPerBlock.x));
        blocksPerGrid.y = (unsigned int)(double(dimension) / double(threadsPerBlock.y));
    }

    create_a <<<blocksPerGrid, threadsPerBlock >>> (dev_left, dev_a_matrix, dimension);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }


    resolve_row <<<blocksPerGrid, threadsPerBlock >>> (dev_a_matrix, dev_b_column,
                                                       dev_prev_step, dev_tmp, dev_res, epsilon,
                                                       dimension);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaStatus = cudaMemcpy(result, dev_res, dimension * sizeof(double), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess)
    {
        cudaFree(dev_left);
        cudaFree(dev_right);
        cudaFree(dev_res);
        cudaFree(dev_b_column);
        cudaFree(dev_a_matrix);
        cudaFree(dev_prev_step);
        return cudaStatus;
    }

    cudaFree(dev_left);
    cudaFree(dev_right);
    cudaFree(dev_res);
    cudaFree(dev_b_column);
    cudaFree(dev_a_matrix);
    cudaFree(dev_prev_step);
    return cudaStatus;
}

__global__ void create_b(const double* left, const double* right, double* b_column)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int dimension = blockDim.x; // размерность определяется размером блока

    if (idx < dimension)
    {
        b_column[idx] = right[idx] / left[idx * dimension + idx];
    }
}


__global__ void create_a(const double* left, double* a_matrix, int dimension)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row == col)
    {
        a_matrix[row * dimension + col] = 0;
    }
    else
    {
        a_matrix[row * dimension + col] = -left[row * dimension + col] / left[row * dimension + row];
    }
}


__global__ void resolve_row(const double* a_matrix, const double *b_column, double* prev_step, double* tmp, double* result, double epsilon, int N)
{
    int ROW = blockIdx.y * blockDim.y + threadIdx.y;

    double tmpSum;

    if (ROW < N) {
        // each thread computes one element of the block sub-matrix
        while(1)
        {
            tmpSum = 0;
            for (int i = 0; i < N; i++) {
                tmpSum += a_matrix[ROW * N + i] * prev_step[i];
            }
            result[ROW] = b_column[ROW] + tmpSum;

            if (result[ROW] > prev_step[ROW])
            {
                if (result[ROW] - prev_step[ROW] < epsilon)
                {
                    break;
                }
            }
            else
            {
                if (prev_step[ROW] - result[ROW] < epsilon)
                {
                    break;
                }
            }

            prev_step[ROW] = result[ROW];
        }
    }
}