# Реализация на GPU посредством openCL
## Сборка
```makefile
gcc -std=c11 -Wall -Wextra -D CL_TARGET_OPENCL_VERSION=100 -fopenmp finder.c -o finderCL -lOpenCL
```
>finder_kernel.cl должен располагаться в той же директории, что и исполняемый файл!
## Запуск
```shell
./finderCL <N>
```