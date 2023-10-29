#ifndef FINDER_FIND_XYZ_H
#define FINDER_FIND_XYZ_H

typedef struct xyz
{
    unsigned int x;
    unsigned int y;
    unsigned int z;
} xyz_t;

int find_xyz(unsigned long long, xyz_t *);

#endif //FINDER_FIND_XYZ_H
