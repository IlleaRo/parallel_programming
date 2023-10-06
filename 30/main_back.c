#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int countPartitions(int n, int* dp) {
    if (n <= 1) {
        return 1;
    }

    if (dp[n] != 0) {
        return dp[n];
    }

    int partitions = 0;
#pragma omp parallel for reduction(+:partitions)
    for (int i = 1; i <= n; ++i) {
        partitions += countPartitions(n - i, dp);
    }

    dp[n] = partitions;
    return partitions;
}

int findMinN(int N) {
    int* dp = (int*)malloc((N + 1) * sizeof(int));
    if (dp == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    int minN = 1;
    int partitions = 0;

#pragma omp parallel for shared(minN, partitions)
    for (int i = 1; i <= N; ++i) {
        dp[i] = countPartitions(i, dp);
        if (dp[i] > N && i < minN) {
#pragma omp critical
            {
                minN = i;
                partitions = dp[i];
            }
        }
    }

    free(dp);
    printf("Minimum n for which p(n) > %d is %d with p(%d) = %d\n", N, minN, minN, partitions);
    return minN;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect arguments! Use ./program <positive integer N>");
        exit(EXIT_FAILURE);
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        fprintf(stderr, "N must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    findMinN(N);

    exit(EXIT_SUCCESS);
}