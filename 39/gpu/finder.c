#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <CL/cl.h>

#define XYZ_COUNT 3
#define NUMBER_OF_THREADS 100
#define MAX_SOURCE_SIZE (0x100000)

int main(int argc,const char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments! Use ./finder <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    char *endptr;
    cl_ulong N = strtoul(argv[1], &endptr, 10);

    if (N == ULONG_MAX || N == 0) {
        fprintf(stderr, "Incorrect arguments! Use ./splitting <64 bit uint>");
        exit(EXIT_FAILURE);
    }

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    /* получить доступные платформы */
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    /* получить доступные устройства */
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    /* создать контекст */
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    /* создаем команду */
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


    cl_program program = NULL;
    cl_kernel kernel = NULL;

    FILE *fp;
    const char fileName[] = "./finder_kernel.cl";
    size_t source_size;
    char *source_str;

	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(EXIT_FAILURE);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    /* скомпилировать программу */
    if ((ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL)) != CL_SUCCESS) {
        fprintf(stderr, "Error when compile kernel (%d)\n", ret);
        
        int log_size;

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, (void *)&log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("Build log:\n%s\n", log);
        free(log);

        exit(EXIT_FAILURE);
    }

    /* создать кернел */
    kernel = clCreateKernel(program, "finder_CL", &ret);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "Error when create kernel (%d)\n", ret);
        exit(EXIT_FAILURE);
    }


    cl_mem memobj = NULL;
    cl_mem mem_for_N = NULL;


    cl_ulong* mem = (cl_ulong *)calloc(XYZ_COUNT, sizeof(cl_ulong));

    for (int i = 0; i < XYZ_COUNT; i++) {
        mem[i] = ULONG_MAX;
    }
    

    /* создать буфер */
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, XYZ_COUNT * sizeof(cl_ulong), NULL, &ret);
    mem_for_N = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_ulong), NULL, &ret);
    

    if (!memobj || !mem_for_N) {
        fprintf(stderr,"Create buf error!\n");
        exit(EXIT_FAILURE);
    }

    /* записать данные в буфер */
    ret = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, XYZ_COUNT * sizeof(cl_ulong), mem, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, mem_for_N, CL_TRUE, 0, sizeof(cl_ulong), &N, 0, NULL, NULL);

    if (ret != CL_SUCCESS) {
        fprintf(stderr,"Write error!\n");
        exit(EXIT_FAILURE);
    }

    /* устанавливаем параметр */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&mem_for_N);
    if (ret != CL_SUCCESS) {
        fprintf(stderr,"Error with clSetKernelArg(%d)!\n", ret);
        exit(EXIT_FAILURE);
    }

    size_t global_work_size[1] = { NUMBER_OF_THREADS };

    for (unsigned long i = N; i < ULONG_MAX; i += NUMBER_OF_THREADS) {
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

        /* считать данные из буфера */
        ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, XYZ_COUNT * sizeof(cl_ulong), mem, 0, NULL, NULL);
        if (mem[0] != ULONG_MAX) {
            break;
        }
        
        ret = clEnqueueWriteBuffer(command_queue, mem_for_N, CL_TRUE, 0, sizeof(cl_ulong), &i, 0, NULL, NULL);
        printf("progress: %ld\n", i);
        sleep(2);
    }
    


    printf("X = %lu\n", mem[0]);
    printf("Y = %lu\n", mem[1]);
    printf("Z = %lu\n", mem[2]);


    // Clean up and wait for all the comands to complete.


    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobj);
    ret = clReleaseMemObject(mem_for_N);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);


    free(mem);

    exit(EXIT_SUCCESS);
}