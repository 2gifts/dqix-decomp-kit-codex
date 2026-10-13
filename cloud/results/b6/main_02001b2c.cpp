#include <globaldefs.h>

// USA: func_02001b2c
extern "C" ARM void func_02001b2c(void* dst, int value, unsigned int n) {
    unsigned char* p = (unsigned char*)dst;
    unsigned int v = (unsigned char)value;

    if (n >= 32) {
        unsigned int head = (0 - (unsigned int)p) & 3;
        if (head != 0) {
            n -= head;
            do {
                *p++ = (unsigned char)v;
            } while (--head != 0);
        }
        if (v != 0) {
            v |= (v << 24) | (v << 16) | (v << 8);
        }
        unsigned int blocks = n >> 5;
        if (blocks != 0) {
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
            } while (--blocks != 0);
        }
        unsigned int words = (n & 0x1f) >> 2;
        if (words != 0) {
            do {
                *(unsigned int*)p = v;
                p += 4;
            } while (--words != 0);
        }
        n &= 3;
    }
    if (n != 0) {
        do {
            *p++ = (unsigned char)v;
        } while (--n != 0);
    }
}
