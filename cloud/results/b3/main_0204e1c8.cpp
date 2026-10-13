#include <globaldefs.h>

struct Wnd0204e1c8 {
    char pad0[8];
    unsigned char* buf;
    char padC[0xa8 - 0xc];
    short width;
    short height;
};

// USA: func_0204e1c8
extern "C" ARM void func_0204e1c8(Wnd0204e1c8* wnd, unsigned char* src, short x, short y,
                                  int w, int h, int color, int mask) {
    if (src == 0) return;
    short width = wnd->width;
    short height = wnd->height;
    for (short j = 0; j < h; j++, y++) {
        short rowTile = y >> 3;
        short rowMod = y & 7;
        short rowOffset = rowMod * 4;
        int rowValid = (rowTile >= 0 && rowTile < height);
        int rowBase = rowTile * wnd->width;
        short col = x;
        for (short i = 0; i < w; i++, col++) {
            short colTile = col >> 3;
            int colValid = (colTile >= 0 && colTile < width);
            short idx = rowBase + colTile;
            int valid = (colValid | rowValid) != 0;
            short colMod = col & 7;
            short off = (colMod >> 1) + (short)(idx * 32);
            off += rowOffset;
            unsigned char* p = wnd->buf + off;
            unsigned char old = *p;
            if (x & 1) {
                if (i & 1) {
                    if (valid) {
                        unsigned char v = *src & color;
                        if (v != 0) { old &= 0xf0; old |= (unsigned)v >> 4; }
                        *p = old;
                    }
                    src++;
                } else {
                    if (valid) {
                        unsigned char v = *src & mask;
                        if (v != 0) { v = v << 4; old &= 0x0f; old |= v; }
                        *p = old;
                    }
                }
            } else {
                if (i & 1) {
                    if (valid) {
                        unsigned char v = *src & color;
                        if (v != 0) { old &= 0x0f; old |= v; }
                        *p = old;
                    }
                    src++;
                } else {
                    if (valid) {
                        unsigned char v = *src & mask;
                        if (v != 0) { old &= 0xf0; old |= v; }
                        *p = old;
                    }
                }
            }
        }
    }
}
