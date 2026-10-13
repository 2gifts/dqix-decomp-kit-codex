#include <globaldefs.h>

struct Grid020b1b54 {
    void* base;             // 0x0
    int field4;             // 0x4
    int field8;             // 0x8
    unsigned char depth;    // 0xc
    unsigned char padD[3];  // 0xd..0xf
    int packed10;           // 0x10
};

struct Ctx020b1b54 {
    int y;
    int depth;
    int tileSize;
    int field4;
    int b0;
    int field8;
    int b1;
    int xAligned;
    int tileX;
    int base;
    int y2Aligned;
    int y2;
    int x2;
    int yAligned;
    int packed10;
};

extern "C" int func_020b10b4(int col, int row, int f4, int f8, unsigned int b0, unsigned int b1);
extern "C" void func_020b11c8(void* addr, int skipX, int skipY, int cols, int rows, unsigned int colorWord, int depth);

// USA: func_020b1b54
ARM void FillGridArea020b1b54(struct Grid020b1b54* g, unsigned int color, int x, int y, int w, int h) {
    const volatile struct Grid020b1b54* g_v = g;
    volatile struct Ctx020b1b54 c;
    c.depth = g_v->depth;

    unsigned int colorWord;
    if (c.depth == 4) {
        colorWord = color;
        colorWord = colorWord | (color << 4);
        colorWord = colorWord | (colorWord << 8);
    } else {
        colorWord = color | (color << 8);
    }
    colorWord = colorWord | (colorWord << 16);

    c.x2 = x + w;
    c.y2 = y + h;
    c.xAligned = x & ~7;
    c.y = y;
    c.yAligned = c.y & ~7;

    c.y2Aligned = (c.y2 + 7) & ~7;
    c.tileSize = (c.depth << 6) / 8;
    c.tileX = c.xAligned / 8;
    int tileY = c.yAligned / 8;

    c.packed10 = g_v->packed10;
    int x2Aligned = (c.x2 + 7) & ~7;

    c.field4 = g_v->field4;
    c.field8 = g_v->field8;
    c.base = (int)g_v->base;
    c.b0 = (unsigned char)(((unsigned int)c.packed10 << 24) >> 24);
    c.b1 = (unsigned char)(((unsigned int)c.packed10 << 16) >> 24);

    if (c.y2Aligned <= c.yAligned) {
        return;
    }

    int newYAligned;
    do {
        int skipY;
        if (c.yAligned >= c.y) {
            skipY = 0;
        } else {
            skipY = c.y - c.yAligned;
        }

        int rows = c.y2 - c.yAligned;
        if (rows > 8) {
            rows = 8;
        }
        rows -= skipY;

        int cx = c.xAligned;
        int tileCol = c.tileX;
        if (cx < x2Aligned) {
            do {
                int tileIdx = func_020b10b4(tileCol, tileY, c.field4, c.field8, c.b0, c.b1);

                int skipX;
                if (cx < x) {
                    skipX = x - cx;
                } else {
                    skipX = 0;
                }

                int cols = c.x2 - cx;
                if (cols > 8) {
                    cols = 8;
                }
                cols -= skipX;

                void* addr = (char*)c.base + c.tileSize * tileIdx;
                func_020b11c8(addr, skipX, skipY, cols, rows, colorWord, c.depth);

                cx += 8;
                tileCol++;
            } while (cx < x2Aligned);
        }

        newYAligned = c.yAligned + 8;
        c.yAligned = newYAligned;
        tileY++;
    } while (newYAligned < c.y2Aligned);
}
