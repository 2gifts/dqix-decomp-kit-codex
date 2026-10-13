#include <globaldefs.h>

typedef int (*Callback020b0f00)(int*, int);

struct Obj020b0f00 {
    void* slot;
    Callback020b0f00 next;
};

struct TextObj020b245c {
    int field_0x0;
    Obj020b0f00* field_0x4;
    int field_0x8;
    int field_0xc;
};

struct TextDirection { signed char x, y; };

extern "C" int func_020b0f00(Obj020b0f00* obj, int base, const char* text, int* out);
extern "C" void func_020b23a8(TextObj020b245c* obj, int x, int y, int arg4, const char* text,
                              const char** textOut, TextDirection dir);

// USA: func_020b245c
extern "C" ARM void func_020b245c(TextObj020b245c* obj, int x, int y, int arg3, int arg4, int flags,
                                  const char* text, TextDirection dir) {
    int lineHeight = obj->field_0xc + ((signed char*)obj->field_0x4->slot)[1];
    int stepX = lineHeight * -dir.y;
    int stepY = lineHeight * dir.x;
    int top;
    int bottom;
    const char* cursor = text;
    if (cursor == 0)
        return;
    top = flags & 0x800;
    bottom = flags & 0x400;
    int px;
    int py;
    int accX = 0;
    int accY = 0;
    do {
        px = x + accX;
        py = y + accY;
        if (top) {
            int n = func_020b0f00(obj->field_0x4, obj->field_0x8, cursor, 0);
            int d = arg3 - n;
            px += d * dir.x;
            py += d * dir.y;
        } else if (bottom) {
            int n = func_020b0f00(obj->field_0x4, obj->field_0x8, cursor, 0);
            int d = (arg3 + 1) / 2 - (n + 1) / 2;
            px += d * dir.x;
            py += d * dir.y;
        }
        func_020b23a8(obj, px, py, arg4, cursor, &cursor, dir);
        accX += stepX;
        accY += stepY;
    } while (cursor != 0);
}
