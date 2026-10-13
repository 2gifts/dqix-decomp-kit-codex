#include <globaldefs.h>
#include <std_library_functions.h>

struct Wnd0204e3e0 {
    char pad0[0xa4];
    int fA4;
    short fA8;
    short fAA;
    char padAC[4];
    short fB0;
    short fB2;
    char padB4[2];
    short fB6;
};

extern "C" void* __clear(void* dst, int count);
extern "C" int func_020420e8(char* str, int id);
extern "C" int _Z27FindEntryIndexByKey020424e4ii(char* key, int tableIdx);
extern "C" char* _Z25FindUnescapedAngleBracketPc(char* str);
extern "C" int _Z23GetBoundedEntry020425b4ii(int idx, int tableIdx);
extern "C" int _Z22LookupKeyValue020425e4iii(int a, int b, int tableIdx);
extern "C" int func_0204fc40(void* wnd, char** cursor, char* tag, short* out, int max);

struct Glyph0204e3e0 {
    int val;
    char width;
    signed char skip : 6;
    signed char unused : 2;
};

extern const unsigned char data_020e7bd6[];
extern char data_020f0248[];
extern char data_020f024e[];
extern char data_020f0251[];
extern char data_020f0254[];
extern char data_020f0258[];
extern char data_020f025b[];
extern char data_020f025e[];
extern char data_020f0298[];
extern char data_020f02d4[];
extern char data_020f02db[];
extern char data_020f02e7[];
extern char data_020f02eb[];
extern char data_020f02f1[];

// USA: func_0204e3e0
extern "C" ARM int func_0204e3e0(Wnd0204e3e0* wnd, char* text, int unused, int center, unsigned char grow, int font) {
    char tmp[0x100];
    short out[8];
    char* p;
    Glyph0204e3e0* glyph;
    int defAdv;
    int prev;
    int code;
    int hasRow;
    int ret;
    int savedX;
    int lineH;
    int rowY;
    int x, y, maxX, maxY;
    int next;
    int ch;
    unsigned char adv;
    char* cursor;
    char* end;

    cursor = text;
    if (cursor == 0) return 0;

    maxX = wnd->fB0;
    maxY = wnd->fB2;
    rowY = 0;
    hasRow = rowY;
    ret = rowY;
    lineH = wnd->fB6;
    x = maxX;
    y = maxY;
    prev = 0xff;
    defAdv = (unsigned char)(data_020e7bd6[font] + 1);

    while ((ch = *cursor) != 0) {
        next = cursor[1];
        if ((ch == '\\' && next == 'n') || (ch == '\r' && next == '\n')) {
            if (maxX < x) maxX = x;
            x = wnd->fB0;
            y += wnd->fB6;
            cursor += 2;
            prev = 0xff;
            continue;
        }
        if (ch == '\n') {
            if (maxX < x) maxX = x;
            x = wnd->fB0;
            y += wnd->fB6;
            cursor += 1;
            prev = 0xff;
            continue;
        }
        int idx = _Z27FindEntryIndexByKey020424e4ii(cursor, font);
        if (*cursor == '<' && idx < 0) {
            p = cursor + 1;
            end = _Z25FindUnescapedAngleBracketPc(cursor);
            if (func_0204fc40(wnd, &p, data_020f0248, out, 1)) {
            } else if (func_0204fc40(wnd, &p, data_020f024e, out, 1)) {
                if (maxX < x) maxX = x;
                x = out[0];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f0251, out, 1)) {
                if (maxY < y) maxY = y;
                y = out[0];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f0254, out, 2)) {
                if (maxX < x) maxX = x;
                if (maxY < y) maxY = y;
                x = out[0];
                y = out[1];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f0258, out, 1)) {
                x += out[0];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f025b, out, 1)) {
                y += out[0];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f025e, out, 2)) {
                x += out[0];
                y += out[1];
                prev = 0xff;
            } else if (func_0204fc40(wnd, &p, data_020f0298, out, 1)) {
                ret = out[0];
            } else if (func_0204fc40(wnd, &p, data_020f02d4, out, 1)) {
                hasRow = 1;
                rowY = out[0];
            } else if (func_0204fc40(wnd, &p, data_020f02db, out, 0)) {
                if (hasRow) {
                    if (maxX < x) maxX = x;
                    if (maxY < y) maxY = y;
                    x = wnd->fB0;
                    y = rowY + wnd->fB2;
                }
            } else if (func_0204fc40(wnd, &p, data_020f02e7, out, 1)) {
                if (maxX < x) maxX = x;
                x = out[0];
                savedX = x;
                prev = 0xff;
                __clear(tmp, 0x100);
                cursor = p;
                char* found = strstr(cursor, data_020f02eb);
                if (found) {
                    int len = found - cursor;
                    if (len < 0x100) {
                        memcpy(tmp, cursor, len);
                        x -= func_020420e8(tmp, font);
                    }
                }
            } else if (func_0204fc40(wnd, &p, data_020f02f1, out, 0)) {
                if (maxX < x) maxX = x;
                x = savedX;
                prev = 0xff;
            }
            cursor = end + 1;
        } else {
            adv = defAdv;
            if (idx < 0) {
                cursor++;
            } else {
                code = idx & 0xff;
                glyph = (Glyph0204e3e0*)_Z23GetBoundedEntry020425b4ii(idx, font);
                adv = glyph->width + 1 + _Z22LookupKeyValue020425e4iii(prev, code, font);
                prev = code;
                cursor += glyph->skip;
            }
            x += adv;
        }
    }

    if (x < maxX) x = maxX;
    if (y < maxY) y = maxY;
    int rem = (x + 3) & 7;
    y = y + lineH - (wnd->fB6 - 10);
    if (rem <= 3) x += 4;
    int w = ((x + 7) & ~7) >> 3;
    int h = ((y + 7) & ~7) >> 3;
    if (w > 0x20) w = 0x20;
    if (h > 0x20) h = 0x20;
    if (grow) {
        if (wnd->fA8 < w) wnd->fA8 = w;
        if (wnd->fAA < h) wnd->fAA = h;
    }
    if (center) {
        y -= wnd->fB2;
        if (wnd->fAA * 8 - y < 7) wnd->fAA = wnd->fAA + 1;
        wnd->fB2 = (wnd->fAA * 8 - y) >> 1;
    }
    if (hasRow) {
        wnd->fB2 = wnd->fB2 + rowY;
        if (center) wnd->fB2 = wnd->fB2 - 1;
    }
    wnd->fA4 = (wnd->fA8 * wnd->fAA) << 5;
    return ret;
}
