#include <globaldefs.h>

struct Window0204f41c {
    char pad0[0xb6];
    short lineSpacing;
};

struct Glyph0204f41c {
    int val;
    signed char width;
    signed char skip : 6;
    signed char unused : 2;
    char pad6[2];
};

extern "C" int func_0204fc40(void* unused0, char** cursor, char* tag, short* out, int max);
extern "C" Glyph0204f41c* _Z22FindEntryByKey0204254cii(int key, int tableIdx);
extern "C" Glyph0204f41c* _Z23GetBoundedEntry020425b4ii(int idx, int tableIdx);
extern "C" void func_0204df9c(Window0204f41c* wnd, Glyph0204f41c* glyph, int x, int y, int color, int font);

extern const unsigned char data_020e7bd8[];
extern char data_020f0248[];
extern char data_020f024e[];
extern char data_020f0251[];
extern char data_020f0254[];
extern char data_020f0258[];
extern char data_020f025b[];
extern char data_020f025e[];
extern char data_020f028c[];

// USA: func_0204f41c
extern "C" ARM void func_0204f41c(Window0204f41c* wnd, short x, short y, char* str, unsigned char height,
                                  unsigned char color, short* maxX, short* maxY, int font) {
    if (!str) return;
    int lineAdv;
    int h;
    short startX;
    short startY;
    unsigned char col;
    char* p = str;
    col = color;
    startX = x;
    startY = y;
    *maxY = 0;
    *maxX = 0;
    h = height;
    if (height == 12) font = 1;
    lineAdv = (unsigned char)(data_020e7bd8[font] + 1);
    short out[8];
    while (*p != 0) {
        int c = *p;
        int c1 = p[1];
        if ((c == '\\' && c1 == 'n') || (c == 0xd && c1 == 0xa)) {
            y = y + wnd->lineSpacing;
            x = startX;
            p += 2;
            continue;
        }
        if (c == 0xa) {
            y = y + wnd->lineSpacing;
            x = startX;
            p += 1;
            continue;
        }
        int adv;
        if (c == '<') {
            p++;
            if (func_0204fc40(wnd, &p, data_020f0248, out, 1)) {
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f0258, out, 1)) {
                x = x + out[0];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f025b, out, 1)) {
                y = y + out[0];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f025e, out, 2)) {
                x = x + out[0];
                y = y + out[1];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f024e, out, 1)) {
                x = out[0];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f0251, out, 1)) {
                y = out[0];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f0254, out, 2)) {
                x = out[0];
                y = out[1];
                continue;
            } else if (func_0204fc40(wnd, &p, data_020f028c, out, 1)) {
                out[0] = out[0] & 0xf;
                col = out[0];
                continue;
            }
            p--;
        }
        adv = lineAdv;
        Glyph0204f41c* glyph = _Z22FindEntryByKey0204254cii((int)p, font);
        if (glyph) {
            func_0204df9c(wnd, glyph, x, y, col, font);
            adv = (unsigned char)(glyph->width + 1);
            short nx = x + adv;
            short ny = y + h;
            if (*maxX < nx) *maxX = nx;
            if (*maxY < ny) *maxY = ny;
            p += glyph->skip;
        } else {
            if (*p != ' ') {
                func_0204df9c(wnd, _Z23GetBoundedEntry020425b4ii(0, font), x, y, col, font);
            }
            p++;
        }
        x = x + adv;
    }
    *maxX = *maxX - startX;
    *maxY = *maxY - startY;
}
