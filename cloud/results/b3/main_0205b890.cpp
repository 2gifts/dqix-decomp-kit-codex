#include <globaldefs.h>

struct Rect0205b1ac {
    int offset;
    short x;
    short y;
    short width;
    short height;
};

struct SpriteAttributes {
    unsigned short yAndShape;
    unsigned short xAndSize;
    unsigned short tileAndPalette;
};

struct SpriteCell {
    unsigned short count;
    char pad2[6];
    SpriteAttributes* sprites;
};

// USA: func_0205b890
extern "C" ARM void func_0205b890(Rect0205b1ac* output, int capacity, SpriteCell* cell) {
    if (!output || !cell) return;
    SpriteAttributes* sprite;
    unsigned short count = cell->count;
    if (capacity < count || !(sprite = cell->sprites)) return;
    int firstX;
    int firstY;
    int firstTile;
    int width;
    int height;
    for (unsigned short index = 0; index < count; ++index, ++sprite) {
        unsigned short shapeBits = sprite->yAndShape;
        unsigned short sizeBits = sprite->xAndSize;
        int y = shapeBits & 0xf;
        int x = sizeBits & 0x1f;
        unsigned short shape = (shapeBits & 0xc000) >> 14;
        unsigned short size = (sizeBits & 0xc000) >> 14;
        switch (shape) {
        case 0:
            if (size == 0) { width = 8; height = width; }
            else if (size == 1) { width = 16; height = width; }
            else if (size == 2) { width = 32; height = width; }
            else if (size == 3) { width = 64; height = width; }
            break;
        case 1:
            if (size == 0) { width = 16; height = 8; }
            else if (size == 1) { width = 32; height = 8; }
            else if (size == 2) { width = 32; height = 16; }
            else if (size == 3) { width = 64; height = 32; }
            break;
        case 2:
            if (size == 0) { width = 8; height = 16; }
            else if (size == 1) { width = 8; height = 32; }
            else if (size == 2) { width = 16; height = 32; }
            else if (size == 3) { width = 32; height = 64; }
            break;
        }
        int tile = sprite->tileAndPalette & 0x3ff;
        if (index == 0) {
            firstX = x;
            firstY = y;
            firstTile = tile;
        }
        output[index].offset = (tile - firstTile) << 5;
        output[index].x = x - firstX;
        output[index].y = y - firstY;
        output[index].width = width;
        output[index].height = height;
    }
}
