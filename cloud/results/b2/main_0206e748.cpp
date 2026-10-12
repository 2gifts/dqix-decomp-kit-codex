#include <globaldefs.h>

#include "std_library_functions.h"

struct Elem0206e748 {
    unsigned char c[3];
    unsigned char a[13];
    unsigned char b[12];
};

struct Obj0206e748 {
    Elem0206e748 cur[5];
    unsigned char pad0[0x334 - 5 * 0x1c];
    Elem0206e748 sel[5];
    unsigned char pad1[0x3c0 - 0x334 - 5 * 0x1c];
    unsigned char bits[0x42];
    unsigned char pad2[0x404 - 0x3c0 - 0x42];
    int f404;
    int f408;
    unsigned char nib[1];
};

// USA: func_0206e748
extern "C" ARM void func_0206e748(Obj0206e748* s, int kind, unsigned idx, unsigned n, int value) {
    if (kind == 0) {
        unsigned i = n >> 3;
        unsigned char mask = 1 << ((int)n % 8);
        if (i < 0x42) {
            if (value) {
                if (s->bits[i] & mask) return;
                s->bits[i] |= mask;
            } else {
                if (!(s->bits[i] & mask)) return;
                s->bits[i] &= ~mask;
            }
        }
    } else if (kind == 1) {
        unsigned i = (n >> 3) + 0x40;
        unsigned char mask = 1 << ((int)n % 8);
        if (i < 0x42) {
            if (value) {
                s->bits[i] |= mask;
            } else {
                s->bits[i] &= ~mask;
            }
        }
        s->f408 = 1;
        return;
    } else if (kind == 2) {
        if (idx < 5) {
            unsigned char r = (value >> 10) & 0x1f;
            unsigned char g = (value >> 5) & 0x1f;
            unsigned char b = value & 0x1f;
            if (r != s->sel[idx].c[0]) {
                memset(s->sel[idx].a, 0, 4);
                memset(s->sel[idx].b, 0, 4);
            } else if (g != s->sel[idx].c[1]) {
                memset(s->sel[idx].b, 0, 4);
            }
            s->sel[idx].c[0] = r;
            s->sel[idx].c[1] = g;
            s->sel[idx].c[2] = b;
            if (value & 0x8000) {
                s->cur[idx].c[0] = r;
                s->cur[idx].c[1] = g;
                s->cur[idx].c[2] = b;
            }
        }
    } else if (kind == 3) {
        if (idx < 5) {
            unsigned i = n >> 3;
            unsigned char mask = 1 << ((int)n % 8);
            if (value) {
                if (s->sel[idx].a[i] & mask) return;
                s->sel[idx].a[i] |= mask;
            } else {
                if (!(s->sel[idx].a[i] & mask)) return;
                s->sel[idx].a[i] &= ~mask;
            }
            s->f408 = 1;
            return;
        }
    } else if (kind == 5) {
        if (idx < 5) {
            unsigned i = n >> 3;
            unsigned char mask = 1 << ((int)n % 8);
            if (value) {
                if (s->sel[idx].b[i] & mask) return;
                s->sel[idx].b[i] |= mask;
            } else {
                if (!(s->sel[idx].b[i] & mask)) return;
                s->sel[idx].b[i] &= ~mask;
            }
            s->f408 = 1;
            return;
        }
    } else if (kind == 7) {
        if (n >= 0xae) {
            unsigned m = (n - 0xae) & 0xff;
            unsigned char odd = (int)m % 2;
            unsigned char* nib = s->nib;
            unsigned char i = m >> 1;
            if (odd) {
                unsigned char v = nib[i];
                unsigned char hi = v >> 4;
                unsigned char t = (hi & 0xc) | value;
                nib[i] = (v & 0xf) | (t << 4);
            } else {
                unsigned char v = nib[i];
                unsigned char lo = v & 0xf;
                lo = (lo & 0xc) | value;
                unsigned char hi = v & 0xf0;
                nib[i] = hi | lo;
            }
        }
    }
    s->f404 = 1;
}
