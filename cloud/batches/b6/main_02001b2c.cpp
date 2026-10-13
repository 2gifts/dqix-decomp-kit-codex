#include <globaldefs.h>

// USA: func_02001b2c
extern "C" ARM void func_02001b2c(void* dst, int value, unsigned int n) {
    unsigned char* p = (unsigned char*)dst;
    unsigned int v = (unsigned char)value;
    unsigned int count;

    if (n >= 32) {
        count = (0 - (unsigned int)p) & 3;
        if (count != 0) {
            n -= count;
            do {
                *p++ = (unsigned char)v;
            } while (--count != 0);
        }
        if (v != 0) {
            v |= (v << 24) | (v << 16) | (v << 8);
        }
        count = n >> 5;
        if (count != 0) {
            do {
                unsigned int* q = (unsigned int*)p;
                q[0] = v;
                q[1] = v;
                q[2] = v;
                q[3] = v;
                q[4] = v;
                q[5] = v;
                q[6] = v;
                q[7] = v;
                p += 32;
            } while (--count != 0);
        }
        count = (n & 0x1f) >> 2;
        if (count != 0) {
            do {
                *(unsigned int*)p = v;
                p += 4;
            } while (--count != 0);
        }
        n &= 3;
    }
    if (n != 0) {
        do {
            *p++ = (unsigned char)v;
        } while (--n != 0);
    }
}
