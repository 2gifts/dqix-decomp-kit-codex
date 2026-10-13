#include <globaldefs.h>
struct Window0204cc68 { char unknown0[8]; unsigned char* pixels; char unknownC[0xa8-0xc]; short width, height; };
// USA: func_0204cc68
extern "C" ARM void func_0204cc68(Window0204cc68* window, unsigned char color, short x, short y, int right, int bottom) {
    if (right < 0 || bottom < 0) return;
    short height = window->height << 3;
    short width = window->width << 3;
    if (right > width) return;
    if (height < bottom) return;
    if (x == right || y == bottom) return;
    unsigned char low = color & 15;
    unsigned char high = low << 4;
    for (; y < bottom; ++y) {
        short tileY = y >> 3;
        int tileRow = tileY * window->width;
        short rowIndex = y & 7;
        short row = rowIndex << 2;
        for (short column = x; column < right; ++column) {
            short tileX = column >> 3;
            short tile = tileRow + tileX;
            short withinTile = column & 7;
            short offset = withinTile >> 1;
            offset = offset + (tile << 5);
            offset += row;
            unsigned char byte = window->pixels[offset];
            if (column & 1) byte = (byte & 15) | high;
            else {
                unsigned char upper = byte & 0xf0;
                byte = upper | low;
            }
            window->pixels[offset] = byte;
        }
    }
}
