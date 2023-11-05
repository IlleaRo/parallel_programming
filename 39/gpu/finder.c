#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <CL/cl.h>
#include <omp.h>

#define KERNEL_PATH "./finder_kernel.cl"
#define XYZ_COUNT 3
#define NUMBER_OF_THREADS 100
#define MAX_SOURCE_SIZE (0x100000)

int main(int argc,const char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    char *end_ptr;
    cl_ulong N = strtoul(argv[1], &end_ptr, 10);

    if (N == ULONG_MAX || N == 0) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    //Initialize needed variables for OpenCL
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    if ((ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms)) != CL_SUCCESS) {
        fprintf(stderr, "Can't get platforms (%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    if ((ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
                         &device_id, &ret_num_devices) != CL_SUCCESS)) {
        fprintf(stderr, "Can't get available devices (%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    cl_context context = clCreateContext(NULL, 1,
                                         &device_id, NULL, NULL, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Context creation failed with an error (%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Context command queue failed with an error (%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    cl_program program = NULL;
    cl_kernel kernel = NULL;

    // Read kernel
    FILE *fp;
    size_t source_size;
    char *source_str;

	fp = fopen(KERNEL_PATH, "r");
	if (!fp) {
		fprintf(stderr, "Can't read kernel file!\n");
		exit(EXIT_FAILURE);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

    program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                        (const size_t *)&source_size, &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Create program failed with an error (%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    // Compilation kernel
    if ((ret = clBuildProgram(program, 1, &device_id,
                              NULL, NULL, NULL)) != CL_SUCCESS) {
        fprintf(stderr, "Compile kernel failed with an error code %d\n", ret);
        
        int log_size;

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                              0, NULL, (void *)&log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                              log_size, log, NULL);
        printf("Build log:\n%s\n", log);
        free(log);

        exit(EXIT_FAILURE);
    }

    kernel = clCreateKernel(program, "finder_CL", &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Creating kernel failed with and error code %d\n", ret);
        exit(EXIT_FAILURE);
    }

    // Working with local variables
    cl_mem mem_for_xyz = NULL;
    cl_mem mem_for_N = NULL;

    cl_ulong* mem = (cl_ulong *)calloc(XYZ_COUNT, sizeof(cl_ulong));

    for (int i = 0; i < XYZ_COUNT; i++) {
        mem[i] = ULONG_MAX;
    }

    mem_for_xyz = clCreateBuffer(context, CL_MEM_READ_WRITE, XYZ_COUNT * sizeof(cl_ulong),
                                 NULL, &ret);
    mem_for_N = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), NULL, &ret);
    

    if (!mem_for_xyz || !mem_for_N) {
        fprintf(stderr,"Create buffer failed with an error code %d!\n", ret);
        exit(EXIT_FAILURE);
    }

    // Writing data to the buffers
    ret = clEnqueueWriteBuffer(command_queue, mem_for_xyz, CL_TRUE, 0,
                               XYZ_COUNT * sizeof(cl_ulong), mem, 0,
                               NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, mem_for_N, CL_TRUE, 0,
                               sizeof(cl_ulong), &N, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        fprintf(stderr,"Writing variables failed with error code %d!\n", ret);
        exit(EXIT_FAILURE);
    }

    // Set variables
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&mem_for_xyz);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_for_N);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_ulong), (void *)&N);

    if (ret != CL_SUCCESS) {
        fprintf(stderr,"Error with clSetKernelArg(%d)!\n", ret);
        exit(EXIT_FAILURE);
    }

    size_t global_work_size[1] = { NUMBER_OF_THREADS }; // Set amount of threads for the one phase

    double start_time, end_time;

    //Search process
    start_time = omp_get_wtime();

    for (unsigned long i = N; i < ULONG_MAX; i += NUMBER_OF_THREADS) {
        clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size,
                               NULL, 0, NULL, NULL);

        clEnqueueReadBuffer(command_queue, mem_for_xyz, CL_TRUE, 0,
                                  XYZ_COUNT * sizeof(cl_ulong), mem, 0,
                                  NULL, NULL);

        if (mem[0] != ULONG_MAX) {
            break;
        }
        
        ret = clSetKernelArg(kernel, 2, sizeof(cl_ulong), (void *)&i);
    }

    end_time = omp_get_wtime();

    if (mem[0] != ULONG_MAX) {
        printf("X = %lu\nY = %lu\nZ = %lu\n", mem[0], mem[1], mem[2]);
    }
    else {
        printf("Something went wrong!\n");
    }

    printf("Execution time: %0.2f sec\n", end_time - start_time);


    // Clean up and wait for all the commands to complete.
    clFlush(command_queue);
    clFinish(command_queue);

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(mem_for_xyz);
    clReleaseMemObject(mem_for_N);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    free(mem);

    exit(EXIT_SUCCESS);
}