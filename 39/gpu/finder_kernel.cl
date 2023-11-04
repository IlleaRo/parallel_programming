
int is_int(double num) {
    return num == (unsigned long) num;
}


int is_square(unsigned long num) {
    double sqrt_num = sqrt((double) num);

    if (is_int(sqrt_num)) {
        return sqrt_num * sqrt_num == num;
    }

    return 0;
}


__kernel
void finder_CL(__global unsigned long* xyz_buf, __global unsigned long *N) {
    int globalId = get_global_id(0);
    unsigned long x, y, z;

    int count_squares = 0;
    x = *N + 1 + globalId;
    printf("X for learn = %lu", x);
    for (y = *N + 1; y < ULONG_MAX && y < x; y++) {
        if (xyz_buf[0] != -1) {
            break;
        }
            
        for (z = *N + 1; z < ULONG_MAX && z < y; z++) {
            count_squares = 0;

            if (xyz_buf[0] != ULONG_MAX) {
                break;
            }

            if (is_square(x + y)) {
                count_squares++;
            }

            if (is_square(x - y)) {
                count_squares++;
            }

            if (is_square(x + z)) {
                count_squares++;
            }

            if (is_square(x - z)) {
                count_squares++;
            }

            if (is_square(y + z)) {
                count_squares++;
            }

            if (count_squares > 4) {
                xyz_buf[0] = x;
                xyz_buf[1] = y;
                xyz_buf[2] = z;
                break;
            }

            if (is_square(y - z)) {
                count_squares++;
            }

            if (count_squares > 4){
                xyz_buf[0] = x;
                xyz_buf[1] = y;
                xyz_buf[2] = z;
                break;
            }
        }
    }

}
