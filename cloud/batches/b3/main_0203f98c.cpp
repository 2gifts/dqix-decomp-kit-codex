#include <globaldefs.h>

extern "C" void func_0203fc28(int s, int t);

extern unsigned short data_020e79d8[];

struct Quad0203f98c {
    unsigned short x;
    unsigned short y;
    unsigned short sizeS;
    unsigned short sizeT;
};

struct Obj0203f98c {
    char pad0[0x2a];
    unsigned short color;
    short field2c;
    short field2e;
    char pad1[0x50 - 0x30];
    unsigned int flags;
};

// USA: func_0203f98c
extern "C" ARM void TODO_Name_0203f98c(struct Obj0203f98c* self, struct Quad0203f98c* quad, unsigned int texAddr, unsigned int palAddr, unsigned short fmt, unsigned short arg6, unsigned short arg7) {
    *(volatile unsigned int*)0x04000444 = 0;
    *(volatile unsigned int*)0x040004a8 = (texAddr >> 3) | (fmt << 26) | 0x40000000 | (quad->sizeS << 20) | (quad->sizeT << 23) | 0x20000000;
    *(volatile unsigned int*)0x040004ac = palAddr >> (4 - (fmt == 2));
    *(volatile unsigned int*)0x040004a4 = 0x80c0 | (self->field2e << 24) | (self->field2c << 16);

    unsigned int w = data_020e79d8[quad->sizeS] << 12;
    unsigned int h = data_020e79d8[quad->sizeT] << 12;
    unsigned int x = quad->x << 12;
    unsigned int y = quad->y << 12;

    if (self->flags & 2) {
        x = (arg6 << 12) - (w + x);
    }
    if (self->flags & 4) {
        y = (arg7 << 12) - (h + y);
    }

    *(volatile unsigned int*)0x04000470 = x;
    *(volatile unsigned int*)0x04000470 = y;
    *(volatile unsigned int*)0x04000470 = 0;
    *(volatile unsigned int*)0x0400046c = w;
    *(volatile unsigned int*)0x0400046c = h;
    *(volatile unsigned int*)0x0400046c = 0x1000;
    *(volatile unsigned int*)0x04000500 = 1;
    *(volatile unsigned int*)0x04000480 = self->color;

    if (self->flags & 2) {
        if (self->flags & 4) {
            func_0203fc28(w, 0);
            *(volatile unsigned int*)0x04000494 = 0x10000000;
            *(volatile unsigned int*)0x04000488 = 0;
            *(volatile unsigned int*)0x04000494 = 0x10001000;
            func_0203fc28(0, h);
            *(volatile unsigned int*)0x04000494 = 0x1000;
            func_0203fc28(w, h);
            *(volatile unsigned int*)0x04000494 = 0;
        } else {
            func_0203fc28(w, h);
            *(volatile unsigned int*)0x04000494 = 0x10000000;
            func_0203fc28(0, h);
            *(volatile unsigned int*)0x04000494 = 0x10001000;
            *(volatile unsigned int*)0x04000488 = 0;
            *(volatile unsigned int*)0x04000494 = 0x1000;
            func_0203fc28(w, 0);
            *(volatile unsigned int*)0x04000494 = 0;
        }
    } else {
        if (self->flags & 4) {
            *(volatile unsigned int*)0x04000488 = 0;
            *(volatile unsigned int*)0x04000494 = 0x10000000;
            func_0203fc28(w, 0);
            *(volatile unsigned int*)0x04000494 = 0x10001000;
            func_0203fc28(w, h);
            *(volatile unsigned int*)0x04000494 = 0x1000;
            func_0203fc28(0, h);
            *(volatile unsigned int*)0x04000494 = 0;
        } else {
            func_0203fc28(0, h);
            *(volatile unsigned int*)0x04000494 = 0x10000000;
            func_0203fc28(w, h);
            *(volatile unsigned int*)0x04000494 = 0x10001000;
            func_0203fc28(w, 0);
            *(volatile unsigned int*)0x04000494 = 0x1000;
            *(volatile unsigned int*)0x04000488 = 0;
            *(volatile unsigned int*)0x04000494 = 0;
        }
    }

    *(volatile unsigned int*)0x04000504 = 0;
    *(volatile unsigned int*)0x04000448 = 1;
}
