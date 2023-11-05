# Реализация на CPU
## Сборка
```makefile
gcc -std=c11 -fopenmp main.c finder_xyz/find_xyz.c -o finder -lm
```
## Запуск
```shell
./finder <N>
```