#ifndef INC_4_LINEAR_SOLVER_GPU_H
#define INC_4_LINEAR_SOLVER_GPU_H
#include </usr/local/cuda/include/cuda_runtime.h>
#include </usr/local/cuda/include/device_launch_parameters.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

cudaError_t resolve_system_gpu(const double*, const double*, double*, int, double);


#endif //INC_4_LINEAR_SOLVER_GPU_H
