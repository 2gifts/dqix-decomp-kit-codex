#include <globaldefs.h>

struct Obj0204f174 {
    char pad0[8];
    void* buf;
    char pad0c[0x90];
    int field9c;
    char pad0a0[4];
    int fA4;
    short fA8;
    short fAA;
    short fAC;
    short fAE;
    char padB0[0x12];
    short fC2;
    char padC4;
    unsigned char flags;
    char padC6[2];
    short fC8;
    short fCA;
    char padCC[4];
    int fD0;
    void* fD4;
};

extern "C" void* memset(void* dst, int value, unsigned int length);
extern "C" void func_0204ea5c(Obj0204f174* obj);
extern "C" void func_0204ecb4(Obj0204f174* obj, int a, int b, int w, int h, signed char* margins);
extern "C" void func_0204f914(Obj0204f174* obj, int mode, short x0, short y0, short x1, short y1);

// USA: func_0204f174
extern "C" ARM void func_0204f174(Obj0204f174* obj, short x, short y, int w, int h, unsigned char c,
                                  void* ptr, unsigned char flag, signed char* margins) {
    obj->fAC = x;
    obj->fAE = y;
    obj->fC2 = c;
    if (ptr != 0) {
        obj->flags |= 4;
    } else {
        obj->flags &= ~4;
    }
    if (flag != 0) {
        obj->flags |= 0x10;
    } else {
        obj->flags &= ~0x10;
    }
    int prod = w * h;
    int area = prod << 1;

    if (obj->flags & 1) {
        if (obj->fA8 < w) w = obj->fA8;
        if (obj->fAA < h) h = obj->fAA;
        obj->fC8 = w;
        obj->fCA = h;
        obj->fD0 = area;
        memset(obj->buf, 0, obj->fA4);
        if (ptr == 0) {
            int oy = 0;
            int sx = (short)(w << 3);
            int ox = 0;
            int sy = (short)(h << 3);
            if (margins != 0) {
                oy = oy + margins[0];
                sy = sy - (margins[0] + margins[1]);
                ox = ox + margins[2];
                sx = sx - (margins[2] + margins[3]);
            }
            func_0204f914(obj, 0x11, (short)ox, (short)oy, (short)sx, (short)sy);
        }
    } else {
        obj->flags |= 1;
        obj->fA8 = w;
        int size = prod << 5;
        obj->fAA = h;
        obj->fA4 = size;
        memset(obj->buf, 0, size);
        if (ptr == 0) {
            short sy = h << 3;
            short sx = w << 3;
            int ox = 0;
            int oy = ox;
            if (margins != 0) {
                oy = ox + margins[0];
                sy = sy - (margins[0] + margins[1]);
                ox = ox + margins[2];
                sx = sx - (margins[2] + margins[3]);
            }
            func_0204f914(obj, 0x11, (short)ox, (short)oy, sx, sy);
        }
        memset(obj->fD4, 0, area);
        obj->fC8 = obj->fA8;
        obj->fCA = obj->fAA;
        obj->fD0 = area;
        obj->field9c = 1;
    }

    if (flag == 0) {
        if (ptr == 0) {
            func_0204ea5c(obj);
        }
        func_0204ecb4(obj, 0, 0, w, h, margins);
    } else {
        obj->flags |= 0x80;
    }
}
