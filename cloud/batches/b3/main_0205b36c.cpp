#include <globaldefs.h>

struct Rect0205b1ac {
    int offset;
    short x;
    short y;
    short width;
    short height;
};

struct GlyphCanvas {
    unsigned char* pixels;
    Rect0205b1ac* regions;
    unsigned char regionCount;
};

struct GlyphDescriptor {
    int field_0x0;
    signed char width;
    char pad5;
    short sourceX;
};

struct FontBitmap {
    int field_0x0;
    unsigned short width;
    char pad6[6];
    unsigned char* pixels;
};

int GetWordFromTable02042638(int);
Rect0205b1ac* FindRectContainingPoint0205b1ac(Rect0205b1ac*, unsigned int, int, int);

// USA: func_0205b36c
extern "C" ARM void func_0205b36c(GlyphCanvas* canvas, GlyphDescriptor* glyph, int x, int y, unsigned char color, unsigned char fontIndex) {
    FontBitmap* font = (FontBitmap*)GetWordFromTable02042638(fontIndex);
    int sourceX = glyph->sourceX;
    unsigned char highColor = color << 4;
    unsigned char lowColor = color & 0xf;
    int rowBytes = font->width >> 3;
    signed char firstBit = 7 - (sourceX & 7);
    unsigned int regionCount = canvas->regionCount;
    Rect0205b1ac* regions = canvas->regions;
    for (int row = 0; row < 12; ++row) {
        short screenY = y + row;
        signed char bit = firstBit;
        unsigned char* source = font->pixels + rowBytes * row + (sourceX >> 3);
        int column = 0;
        for (; column < glyph->width; ++column) {
            if (*source & (1 << bit)) {
                short screenX = x + column;
                Rect0205b1ac* region = FindRectContainingPoint0205b1ac(regions, regionCount, screenX, screenY);
                if (region) {
                    int oddPixel = screenX & 1;
                    screenX -= region->x;
                    unsigned char* destination = canvas->pixels + region->offset;
                    short localY = screenY - region->y;
                    short tileIndex = (short)(localY >> 3) * (short)(region->width >> 3) + (short)(screenX >> 3);
                    destination += tileIndex << 5;
                    short pixelX = screenX & 7;
                    short pixelY = localY & 7;
                    int pixel = (pixelX >> 1) + (pixelY << 2);
                    if (oddPixel) {
                        destination[pixel] &= 0xf;
                        destination[pixel] |= highColor;
                    } else {
                        destination[pixel] &= 0xf0;
                        destination[pixel] |= lowColor;
                    }
                }
            }
            --bit;
            if (bit < 0) {
                bit = 7;
                ++source;
            }
        }
    }
}
