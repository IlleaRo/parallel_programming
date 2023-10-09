#ifndef INC_4_LINEAR_SOLVER_CPU_H
#define INC_4_LINEAR_SOLVER_CPU_H

// 1 - incorrect arguments
// 2 - malloc/calloc error
// 3 - incorrect matrix

int resolve_system_cpu(const double* les_l, const double* les_r, double* result, int dimension, double epsilon);
int diagonal_dominance(const double *A, int dimension);
#endif //INC_4_LINEAR_SOLVER_CPU_H



