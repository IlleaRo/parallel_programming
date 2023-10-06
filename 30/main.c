#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define ARR_SIZE 1459

__uint128_t read(const char *buf, int size) {

    if (!buf) return -1;

    __uint128_t x = 0;

    for (int i = 0; i < size; ++i)
    {
        if (buf[i] == '\0')
            break;
        if (buf[i] >= '0' && buf[i] <= '9')
        {
            if (x > x * 10 + buf[i] - '0')
            {
                return -1;
            }
            x = x * 10 + buf[i] - '0';
        }
    }
    return x;
}

void print_uint128(__uint128_t num) {
    if (num > 9) {
        print_uint128(num / 10);
    }
    putchar((char)('0' + num % 10));
}

typedef struct result_of_splitting
{
    unsigned short n;
    __uint128_t p;
} result_of_splitting_t;

result_of_splitting_t* findMinN_parallel(__uint128_t N, result_of_splitting_t* result) {
    __uint128_t np[ARR_SIZE] = {0};

    np[0] = 1;
    unsigned short n = 1;

    //int nthreads, tid;

        while (1)
        {
            #pragma omp parallel for ordered  default(none)   /*private(nthreads, tid)*/ shared(n, np)
                    for (int i = n; i < ARR_SIZE; i++) {
                        //printf("Hello from process: %d\n", omp_get_thread_num());
                        #pragma omp ordered
                        {
                            np[i] += np[i - n];
                        }
                    }


            if (np[n] > N)
            {
                result->n = n;
                result->p = np[n];
                return result;
            }

            if (n == ARR_SIZE - 1) {
                result->n = n + 1;
                result->p = -1;
                return result;
            }

            n++;
        }
}

result_of_splitting_t* findMinN(__uint128_t N, result_of_splitting_t* result)
{
    __uint128_t np[ARR_SIZE] = { 0 };

    np[0] = 1;
    unsigned short n = 1;

    while (1)
    {

        np[n] += np[0];

        if (np[n] > N)
        {
            result->n = n;
            result->p = np[n];
            return result;
        }


        for (int i = n+1; i < ARR_SIZE; i++)
        {
            np[i] += np[i - n];
        }


        if (n == ARR_SIZE-1)
        {
            result->n = n+1;
            result->p = -1;
            return result;
        }

        n++;
    }
}


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Incorrect arguments! Use ./splitting <positive integer (long)>");
        exit(EXIT_FAILURE);
    }

    __uint128_t N;
    N = read(argv[1], strlen(argv[1])+1);
    if (N == -1)
    {
        fprintf(stderr, "Incorrect arguments! Use ./splitting <128 bit uint>");
        exit(EXIT_FAILURE);
    }


    result_of_splitting_t res = {0};
    clock_t start_time = clock();

    findMinN(N, &res);
    clock_t end_time = clock();

    printf("p(%d) = ", res.n);
    print_uint128(res.p);
    printf("%c will greater than ", res.p == -1 ? '+' : ' ');
    print_uint128(N);
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf(" TIME: %f секунд\n", elapsed_time);

    start_time = clock();
    findMinN_parallel(N, &res);
    end_time = clock();
    printf("p(%d) = ", res.n);
    print_uint128(res.p);
    printf("%c will greater than ", res.p == -1 ? '+' : ' ');
    print_uint128(N);
    elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf(" TIME: %f секунд\n", elapsed_time);


    exit(EXIT_SUCCESS);
}