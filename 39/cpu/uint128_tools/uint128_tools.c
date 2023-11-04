#include <stdio.h>

__uint128_t read(const char *buf, unsigned long size) {

    if (!buf) {
        return -1;
    }

    __uint128_t x = 0;

    for (int i = 0; i < size; ++i) {
        if (buf[i] == '\0') {
            break;
        }

        if (buf[i] >= '0' && buf[i] <= '9') {
            if (x > x * 10 + buf[i] - '0') {
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