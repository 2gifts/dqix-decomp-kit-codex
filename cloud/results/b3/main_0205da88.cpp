#include <globaldefs.h>

struct Entry0205da88 {
    char pad0[0x1c];
    unsigned char lowNibble : 4;
    unsigned char highNibble : 4;
    char pad1[0x3];
};

struct ElemTarget0205da88 {
    char pad0[0x4];
    struct Entry0205da88* matched;
    char pad1[0xa0];
    short w;
    char pad2[0x2];
    short y;
    short x;
    char pad3[0x10];
    short h;
    char pad4[0x3];
    unsigned char flags;
    char pad5[0x1a];
};

struct Obj0205da88 {
    char pad0[0x98];
    struct Entry0205da88* entries;
    struct ElemTarget0205da88* targets;
    char pad1[0x12];
    unsigned char entryCount;
    unsigned char targetCount;
};

int CheckField0x9cSetWhenField0xd4Present(unsigned char* obj);
extern "C" int func_0204fd00(struct ElemTarget0205da88* elem, int mask);
extern "C" void func_0204bc74(struct Entry0205da88* obj, unsigned short tile, int x, int y, int w, int h, unsigned short palette);

// USA: func_0205da88
ARM void DrawMatchedElementFrame0205da88(struct Obj0205da88* obj, int a1, int a2, unsigned short tile) {
    if (obj->entries != NULL && obj->targets != NULL) {
        struct Entry0205da88* entry1;
        struct Entry0205da88* entry2;
        struct ElemTarget0205da88* elem;
        unsigned char i;

        a1 &= 3;
        a2 &= 3;
        if (a1 == a2) return;

        entry1 = NULL;
        entry2 = NULL;
        for (i = 0; i < obj->entryCount; i++) {
            struct Entry0205da88* entry = obj->entries + i;
            if (a1 == entry->highNibble) entry1 = entry;
            if (a2 == entry->highNibble) entry2 = entry;
        }
        if ((entry1 == NULL) | (entry2 == NULL)) return;

        tile &= 0x3ff;
        for (i = 0; i < obj->targetCount; i++) {
            elem = obj->targets + i;
            if (!CheckField0x9cSetWhenField0xd4Present((unsigned char*)elem)) continue;
            if (elem->flags & 0x20) continue;
            if (elem->flags & 0x80) continue;
            if (entry2 != elem->matched) continue;

            short y = elem->y;
            int x = elem->x;
            short w = elem->w;
            short h = elem->h;
            func_0204bc74(entry1, tile, x, y, w, h, 0);
            if (h == 0 || tile == 0) continue;

            if (!func_0204fd00(elem, 1)) func_0204bc74(entry1, 2, x, y, 1, 1, 0);
            if (!func_0204fd00(elem, 2)) func_0204bc74(entry1, 0x402, (short)(x + w - 1), y, 1, 1, 0);
            if (!func_0204fd00(elem, 4)) func_0204bc74(entry1, 0x802, x, (short)(y + h - 1), 1, 1, 0);
            if (!func_0204fd00(elem, 8)) func_0204bc74(entry1, 0xC02, (short)(x + w - 1), (short)(y + h - 1), 1, 1, 0);
        }
    }
}
