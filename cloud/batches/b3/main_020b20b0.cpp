#include <globaldefs.h>

struct Cell020b20b0 {
    unsigned int word0;
    unsigned short half4;
    unsigned short half6;
};

extern unsigned char data_020e91e0[4][4][2];
int LookupWord2D(unsigned char* p);

// USA: func_020b20b0
extern "C" ARM int func_020b20b0(struct Cell020b20b0* out, int w, int h, struct Cell020b20b0* xs, struct Cell020b20b0* ys, int flag, unsigned int off, int shift) {
    int wLog = 3;
    int hLog = 3;
    if (w < 8)
        wLog = (w >= 4) ? 2 : ((w >= 2) ? 1 : 0);
    if (h < 8)
        hLog = (h >= 4) ? 2 : ((h >= 2) ? 1 : 0);

    unsigned char* tile = data_020e91e0[hLog][wLog];
    int lookup = LookupWord2D(tile);
    int tx = tile[0];
    int ty = tile[1];

    unsigned int wFull = w & (~0u << tx);
    unsigned int hFull = h & (~0u << ty);
    int m = flag ? 2 : 1;
    int area = m << (tx + ty);
    int nX = w >> tx;
    int nY = h >> ty;
    int total = 0;

    for (int j = 0; j < nY; j++) {
        unsigned int yBits = (unsigned int)(ys + (j << ty)) & 0xff;
        for (int i = 0; i < nX; i++) {
            unsigned int xBits = (unsigned int)(xs + (i << tx)) & 0x1ff;
            out->word0 = (((out->word0 & 0xfe00ff00) | yBits | (xBits << 16)) & 0x3fff3fff) | lookup;
            out->half4 = (out->half4 & 0xfc00) | off;
            off += area >> shift;
            out->word0 = (out->word0 & ~0x2000) | (flag << 13);
            out++;
        }
    }
    total += nX * nY;

    if (wFull < (unsigned int)w) {
        unsigned int remW = w - wFull;
        int n = func_020b20b0(out, remW, hFull, xs + wFull, ys, flag, off, shift);
        out += n;
        off += (hFull * (m * remW)) >> shift;
        total += n;
    }

    if (hFull < (unsigned int)h) {
        unsigned int remH = h - hFull;
        int n = func_020b20b0(out, wFull, remH, xs, ys + hFull, flag, off, shift);
        out += n;
        off += (remH * (m * wFull)) >> shift;
        total += n;
    }

    if (wFull < (unsigned int)w && hFull < (unsigned int)h) {
        int n = func_020b20b0(out, w - wFull, h - hFull, xs + wFull, ys + hFull, flag, off, shift);
        total += n;
    }

    return total;
}
