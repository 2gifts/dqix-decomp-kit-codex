#include <globaldefs.h>

struct TileFillCtx020b18e8 {
    unsigned char* dest;
    int width;
    int height;
    unsigned char depth;
    int tileStride;
};

extern "C" void func_020b11c8(unsigned char*, int, int, int, int, unsigned int, int);

// USA: func_020b19b8
extern "C" ARM void func_020b19b8(TileFillCtx020b18e8* ctx, unsigned int color, int x, int y, int width, int height) {
    int depth = ctx->depth;
    width += x;
    height += y;
    if (depth == 4) {
        color |= color << 4;
        color |= color << 8;
        color |= color << 16;
    } else {
        color |= color << 8;
        color |= color << 16;
    }
    int tileBytes = (depth << 6) / 8;
    int firstY = y & ~7;
    int firstX = x & ~7;
    unsigned char* row = ctx->dest + tileBytes * ((firstY / 8) * ctx->tileStride + firstX / 8);
    int roundedEndY = (height + 7) & ~7;
    int roundedEndX = (width + 7) & ~7;
    int rowBytes = ctx->tileStride * tileBytes;
    int tileY = firstY;
    if (tileY < roundedEndY) do {
        int offsetY = tileY < y ? y - tileY : 0;
        int remainingY = height - tileY;
        if (remainingY > 8) remainingY = 8;
        int fillHeight = remainingY - offsetY;
        unsigned char* tile = row;
        int tileX = firstX;
        if (tileX < roundedEndX) do {
            int offsetX = tileX < x ? x - tileX : 0;
            int remainingX = width - tileX;
            if (remainingX > 8) remainingX = 8;
            func_020b11c8(tile, offsetX, offsetY, remainingX - offsetX, fillHeight, color, depth);
            tile += tileBytes;
            tileX += 8;
        } while (tileX < roundedEndX);
        row += rowBytes;
        tileY += 8;
    } while (tileY < roundedEndY);
}
