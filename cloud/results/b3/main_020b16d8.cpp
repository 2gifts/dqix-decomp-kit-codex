#include <globaldefs.h>

struct TileCtx020b16d8 {
    void* dest;
    unsigned int width;
    unsigned int height;
    unsigned char depth;
    unsigned char padD[3];
    unsigned int flags;
};

struct TileBlob020b16d8 {
    unsigned char b0;
    unsigned char b1;
    unsigned char pad2[4];
    unsigned char b6;
};

struct TileSet020b16d8 {
    int pad0;
    int pad4;
    struct TileBlob020b16d8* blob;
};

struct TileSrc020b16d8 {
    struct TileSet020b16d8* set;
};

struct TileRef020b16d8 {
    unsigned char* tile;
    int field4;
};

struct BlitParams020b16d8 {
    char* dest;
    int field04;
    int x;
    int y;
    int w;
    int h;
    int area;
    int b6;
    int depth;
    int last;
};

extern "C" int func_020b10b4(int tx, int ty, int w, int h, int lo, int hi);
extern "C" void func_020b1300(struct BlitParams020b16d8* params);

// USA: func_020b16d8
ARM void BlitTiles020b16d8(struct TileCtx020b16d8* ctx, struct TileSrc020b16d8* src,
                           int x, int y, int n, struct TileRef020b16d8* ref) {
    struct BlitParams020b16d8 params;
    int depth = ctx->depth;
    int unit = (depth << 6) / 8;
    int w = ref->tile[1];
    int h = src->set->blob->b1;
    unsigned int cols = ctx->width;
    unsigned int rows = ctx->height;
    if (w == 0) {
        return;
    }
    int x1 = x + w;
    if (x1 < 0) {
        return;
    }
    int y1 = y + h;
    if (y1 < 0) {
        return;
    }
    unsigned int tx0 = x > 0 ? (unsigned int)x >> 3 : 0;
    unsigned int ty0 = y > 0 ? (unsigned int)y >> 3 : 0;
    unsigned int tx1 = (unsigned int)(x1 + 7) >> 3;
    if (cols <= tx1) {
        tx1 = cols;
    }
    unsigned int ty1 = (unsigned int)(y1 + 7) >> 3;
    if (rows <= ty1) {
        ty1 = rows;
    }
    int nx = tx1 - tx0;
    if (nx < 0) {
        return;
    }
    int ny = ty1 - ty0;
    if (ny < 0) {
        return;
    }
    char* dest = (char*)ctx->dest;
    if (x >= 0) {
        x &= 7;
    }
    int field4 = ref->field4;
    if (y >= 0) {
        y &= 7;
    }
    int xEnd = x - nx * 8;
    int yEnd = y - ny * 8;
    params.field04 = field4;
    params.w = w;
    params.h = h;
    params.b6 = src->set->blob->b6;
    params.depth = ctx->depth;
    params.area = src->set->blob->b6 * src->set->blob->b0;
    params.last = n - 1;
    int row = ty0;
    for (; y > yEnd; y -= 8) {
        int col = tx0;
        int cx = x;
        params.y = y;
        for (; cx > xEnd; cx -= 8) {
            int t = func_020b10b4(col, row, ctx->width, ctx->height, (unsigned char)ctx->flags,
                                  (unsigned char)(ctx->flags >> 8));
            params.dest = dest + t * unit;
            params.x = cx;
            func_020b1300(&params);
            col++;
        }
        row++;
    }
}
