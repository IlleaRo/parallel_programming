#include <stdio.h>
#include <stdlib.h>


void multiply(const double *a, const double *b, double *res, int dimension)
{
    for (int i = 0; i < dimension; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < dimension; ++j)
        {
            res[i]+= b[j] * *(a+i*dimension+j);
        }
    }
}

int check(const double *a, const double *b, int dimension, double epsilon)
{
    char res = 0;
    for (int i = 0; i < dimension && !res; ++i)
    {
        if (a[i] > b[i])
        {
            if (a[i] - b[i] < epsilon)
            {
                res++;
            }
        }
        else
        {
            if (b[i] - a[i] < epsilon)
            {
                res++;
            }
        }
    }
    return !res;
}


int main() {

    FILE* file;

    if ((file = fopen("task","r")) == NULL)
    {
        fprintf(stderr, "Can't find \"task\"\n");
        exit(EXIT_FAILURE);
    }

    int dimension;

    if (fscanf(file,"%d", &dimension) == 0)
    {
        fprintf(stderr, "Incorrect data in file!\n");
        exit(EXIT_FAILURE);
    }

    double* les_l = (double*) malloc(sizeof(double) * dimension * dimension);
    double* les_r = (double*) malloc(sizeof(double) * dimension);
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

    for (int i = 0; i < dimension; ++i) {
        if (fscanf(file,"%lf", &les_r[i]) == 0)
        {
            fprintf(stderr, "Incorrect data in file!\n");
            exit(EXIT_FAILURE);
        }
    }

    double epsilon = 0.00001;

    if (fscanf(file,"%lf", &epsilon) == 0)
    {
        fprintf(stderr, "Incorrect data in file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    // Создание матрицы b
    double b_column[dimension];
    for (int i = 0; i < dimension; ++i)
    {
        b_column[i] = les_r[i] / *(les_l + i * dimension + i);
    }

    // Создание матрицы a
    double *a_matrix = (double*) malloc(sizeof(double) * dimension * dimension);
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            if (i == j)
            {
                *(a_matrix + i * dimension + j) = 0;
                continue;
            }
            *(a_matrix + i * dimension + j) = - *(les_l + i * dimension + j) / *(les_l + i * dimension + i);
        }
    }

    free(les_l);
    free(les_r);

    // Результат
    double result[] = {1000, 1000, 1000, 1000};
    double prev_step[] = {1000, 1000, 1000, 1000};
    double tmp[dimension];

    for (int i = 0; i < 501; ++i)
    {
        if (check(result, prev_step, dimension, epsilon) == 1)
        {
            break;
        }
        for (int j = 0; j < dimension; ++j)
        {
            multiply(a_matrix, prev_step, tmp, dimension);
            result[j] = b_column[j] + tmp[j];
            prev_step[j] = result[j];
        }
    }

    for (int i = 0; i < dimension; ++i)
    {
        printf("%lf\n", result[i]);
    }

    exit(EXIT_SUCCESS);
}
