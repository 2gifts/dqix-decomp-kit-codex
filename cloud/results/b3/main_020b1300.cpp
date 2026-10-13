#include <globaldefs.h>

struct BitReader020b29f8 {
    unsigned char* data;
    signed char count;
    unsigned char buf;
};

int ReadBits020b29f8(struct BitReader020b29f8* r, int n);

struct TileBits020b1300 {
    unsigned int* dest;
    unsigned char* src;
    int x;
    int y;
    int w;
    int h;
    int rowBits;
    int bits;
    int bpp;
    int bias;
};

// USA: func_020b1300
ARM void UnpackTile020b1300(struct TileBits020b1300* ctx) {
    struct BitReader020b29f8 rdB;
    struct BitReader020b29f8 rdA;
    int start = ctx->x < 0 ? 0 : ctx->x;
    int end = ctx->x + ctx->w;
    int y0 = ctx->y < 0 ? 0 : ctx->y;
    int y1 = ctx->y + ctx->h;
    if (end >= 8) {
        end = 8;
    }
    if (y1 >= 8) {
        y1 = 8;
    }
    int negCy = -(ctx->y > 0 ? 0 : ctx->y);
    int negCx = -(ctx->x > 0 ? 0 : ctx->x);
    int bpp = ctx->bpp;
    end = end * bpp;
    int sb = ctx->bits * negCx;
    int rowBits = ctx->rowBits;
    start = start * bpp;
    int cursor = negCy * rowBits + sb;
    unsigned char* src = ctx->src;
    unsigned int* dest = ctx->dest;

    if (bpp == 4) {
        unsigned int* p = dest + y0;
        unsigned int* pe = dest + y1;
        if (p >= pe) {
            return;
        }
        int bias = ctx->bias;
        while (p < pe) {
            rdA.data = src + ((unsigned int)cursor >> 3);
            rdA.count = 0;
            rdA.buf = 0;
            ReadBits020b29f8(&rdA, cursor % 8);
            unsigned int word = *p;
            for (int pos = start; pos < end; pos += 4) {
                int v = ReadBits020b29f8(&rdA, ctx->bits);
                if (v != 0) {
                    v += bias;
                    word = (word & ~(0xf << pos)) | (v << pos);
                }
            }
            *p++ = word;
            cursor += rowBits;
        }
        return;
    }

    unsigned int* p = dest + y0 * 2;
    unsigned int* pe = dest + y1 * 2;
    if (p >= pe) {
        return;
    }
    int bias = ctx->bias;
    while (p < pe) {
        rdB.data = src + ((unsigned int)cursor >> 3);
        rdB.count = 0;
        rdB.buf = 0;
        ReadBits020b29f8(&rdB, cursor % 8);
        unsigned int lo = p[0];
        unsigned int hi = p[1];
        for (int pos = start; pos < end; pos += 8) {
            int v = ReadBits020b29f8(&rdB, ctx->bits);
            if (v != 0) {
                v += bias;
                if (pos < 32) {
                    lo = (lo & ~(0xff << pos)) | (v << pos);
                } else {
                    hi = (hi & ~(0xff << (pos - 32))) | (v << (pos - 32));
                }
            }
        }
        p[0] = lo;
        p[1] = hi;
        p += 2;
        cursor += rowBits;
    }
}
