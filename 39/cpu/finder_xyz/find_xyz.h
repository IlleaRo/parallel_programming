#ifndef FINDER_FIND_XYZ_H
#define FINDER_FIND_XYZ_H

typedef struct xyz
{
    unsigned long x;
    unsigned long y;
    unsigned long z;
} xyz_t;

int find_xyz(unsigned long, xyz_t *);

#endif //FINDER_FIND_XYZ_H
