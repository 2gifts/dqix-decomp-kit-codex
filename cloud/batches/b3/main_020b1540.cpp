#include <globaldefs.h>

struct TileSurface {
    unsigned char* pixels;
    unsigned int width;
    unsigned int height;
    unsigned char bitsPerPixel;
    char pad_d[3];
    int stride;
};
struct FontMetrics {
    unsigned char width;
    unsigned char height;
    char pad_2[4];
    unsigned char bitsPerPixel;
};
struct FontResource { char pad_0[8]; FontMetrics* metrics; };
struct Font { FontResource* resource; };
struct GlyphMetrics { unsigned char field_0; unsigned char width; };
struct Glyph { GlyphMetrics* metrics; void* pixels; };
struct GlyphBlit {
    unsigned char* destination;
    void* source;
    int x;
    int y;
    int width;
    int height;
    int sourceStride;
    int sourceBits;
    int destinationBits;
    int color;
};
extern "C" void func_020b1300(GlyphBlit*);

// USA: func_020b1540
extern "C" ARM void func_020b1540(TileSurface* surface, Font* font, int x, int y, int color, Glyph* glyph) {
    int tileBytes = (surface->bitsPerPixel * 64) / 8;
    int glyphWidth = glyph->metrics->width;
    unsigned int surfaceWidth = surface->width;
    unsigned int surfaceHeight = surface->height;
    unsigned char* pixels = surface->pixels;
    int glyphHeight = font->resource->metrics->height;
    if (!glyphWidth) return;
    int right = x + glyphWidth;
    if (right < 0) return;
    int bottom = y + glyphHeight;
    if (bottom < 0) return;
    int firstX = x <= 0 ? 0 : (unsigned int)x / 8;
    int firstY = y <= 0 ? 0 : (unsigned int)y / 8;
    int lastX = (unsigned int)(right + 7) / 8;
    if (surfaceWidth <= (unsigned int)lastX) lastX = surfaceWidth;
    int lastY = (unsigned int)(bottom + 7) / 8;
    if (surfaceHeight <= (unsigned int)lastY) lastY = surfaceHeight;
    int columns = lastX - firstX;
    int rows = lastY - firstY;
    if (columns < 0) return;
    if (rows < 0) return;
    int rowSkip = tileBytes * (surface->stride - columns);
    unsigned char* destination = pixels + tileBytes * (surface->stride * firstY + firstX);
    GlyphBlit blit;
    blit.source = glyph->pixels;
    blit.color = color - 1;
    blit.width = glyphWidth;
    blit.height = glyphHeight;
    blit.sourceBits = font->resource->metrics->bitsPerPixel;
    blit.destinationBits = surface->bitsPerPixel;
    blit.sourceStride = (short)blit.sourceBits * (short)font->resource->metrics->width;
    if (x >= 0) x &= 7;
    if (y >= 0) y &= 7;
    int stopY = y - rows * 8;
    int stopX = x - columns * 8;
    if (y <= stopY) return;
    do {
        blit.y = y;
        int currentX = x;
        if (currentX > stopX) do {
            blit.destination = destination;
            blit.x = currentX;
            func_020b1300(&blit);
            currentX -= 8;
            destination += tileBytes;
        } while (currentX > stopX);
        y -= 8;
        destination += rowSkip;
    } while (y > stopY);
}
