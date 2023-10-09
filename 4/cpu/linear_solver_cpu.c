#include "linear_solver_cpu.h"


#include <stdlib.h>
#include <math.h>


int diagonal_dominance(const double *A, int dimension)
{
    // Проверяем диагональное преобладание
    int is_strong = 0;
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            if (i == j)
            {
                continue;
            }

            if (fabs(*(A+i * dimension + j)) > fabs(*(A + i * dimension + i)))
            {
                return 0;
            }

            if (!is_strong)
            {
                if (fabs(*(A+i * dimension + j)) < fabs(*(A + i * dimension + i)))
                {
                    is_strong = 1;
                }
            }
        }
    }


    if (!is_strong)
    {
        return 0;
    }

    return 1;
}

void multiply(const double* a, const double* b, double* res, int dimension)
{
    for (int i = 0; i < dimension; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < dimension; ++j)
        {
            res[i] += b[j] * *(a + i * dimension + j);
        }
    }
}

int check(const double* a, const double* b, int dimension, double epsilon)
{
    for (int i = 0; i < dimension; ++i)
    {
        if (a[i] > b[i])
        {
            if (a[i] - b[i] >= epsilon)
            {
                return 0;
            }
        }
        else
        {
            if (b[i] - a[i] >= epsilon)
            {
                return 0;
            }
        }
    }

    return 1;
}

int resolve_system_cpu(const double* les_l, const double* les_r, double* result, int dimension, double epsilon)
{

    if (!les_l || !les_r || !result)
    {
        return 1;
    }


    // Создание матрицы b
    double* b_column = (double*)malloc(sizeof(double) * dimension);

    if (!b_column)
    {
        return 2;
    }

    for (int i = 0; i < dimension; ++i)
    {
        b_column[i] = les_r[i] / *(les_l + i * dimension + i);
    }

    // Создание матрицы a
    double* a_matrix = (double*)malloc(sizeof(double) * dimension * dimension);

    if (!a_matrix)
    {
        return 2;
    }

    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            if (i == j)
            {
                *(a_matrix + i * dimension + j) = 0;
                continue;
            }
            *(a_matrix + i * dimension + j) = -*(les_l + i * dimension + j) / *(les_l + i * dimension + i);
        }
    }

    // Результат
    double* prev_step = (double*)calloc(dimension, sizeof(double));



    double* tmp = (double*)calloc(dimension, sizeof(double));

    if (!tmp || !prev_step)
    {
        return 2;
    }

    for (int i = 0; i < dimension; i++)
    {
        prev_step[i] = 1000;
        result[i] = 1000;
    }

    while (1)
    {
        multiply(a_matrix, prev_step, tmp, dimension);
        for (int j = 0; j < dimension; ++j)
        {
            result[j] = b_column[j] + tmp[j];
        }

        if (check(result, prev_step, dimension, epsilon))
        {
            break;
        }

        for (int j = 0; j < dimension; ++j)
        {
            prev_step[j] = result[j];
        }
    }

    free(a_matrix);
    free(b_column);
    free(tmp);
    free(prev_step);

    return 0;
}

