#include <globaldefs.h>

extern "C" int _Z20IsWithinRect0204ea28iiiiss(int px, int py, int x0, int y0, short x1, short y1);

struct FlagsRef0204fd00 {
    char pad[0x1c];
    unsigned char lo : 4;
    unsigned char hi : 4;
};

struct Obj0204fd00 {
    Obj0204fd00* next;
    FlagsRef0204fd00* ref;
    char pad0[0xa8 - 0x8];
    short w;
    short h;
    short x;
    short y;
    char pad1[0xc5 - 0xb0];
    unsigned char flags;
};

// USA: func_0204fd00
extern "C" ARM int func_0204fd00(Obj0204fd00* self, int mask) {
    Obj0204fd00* item = self->next;
    if (item == NULL) return 0;
    int selfX = self->x;
    short selfW = self->w;
    short selfH = self->h;
    short selfY = self->y;
    int selfRight = selfX + selfW;
    int selfNib = 0;
    if (self->ref != NULL) selfNib = self->ref->lo;
    short selfBottom = selfY + selfH;
    short selfRightM1 = selfRight - 1;
    short selfBottomM1 = selfBottom - 1;
    int result = 0;
    for (; item != NULL; item = item->next) {
        if (item->ref != NULL && self->ref != NULL && item->ref->lo != selfNib) continue;
        if (item->flags & 0x20) continue;
        if ((item->flags & 4) && (item->flags & 0x10)) continue;
        short itemX = item->x;
        short itemW = item->w;
        short itemY = item->y;
        short itemH = item->h;
        short itemRight = itemX + itemW;
        short itemBottom = itemY + itemH;
        if (mask & 1) {
            if (_Z20IsWithinRect0204ea28iiiiss(selfX, selfY, itemX, itemY, itemRight, itemBottom)) {
                if (selfX != itemX || selfY != itemY) {
                    result = 5;
                    if (selfX == itemX) { result = 1; break; }
                    if (selfY == itemY) result = 2;
                    break;
                }
            }
        }
        if (mask & 2) {
            if (_Z20IsWithinRect0204ea28iiiiss(selfRightM1, selfY, itemX, itemY, itemRight, itemBottom)) {
                if (selfRight != itemRight || selfY != itemY) {
                    result = 5;
                    if (selfRight == itemRight) { result = 3; break; }
                    if (selfY == itemY) result = 2;
                    break;
                }
            }
        }
        if (mask & 4) {
            if (_Z20IsWithinRect0204ea28iiiiss(selfX, selfBottomM1, itemX, itemY, itemRight, itemBottom)) {
                if (selfX != itemX || selfBottom != itemBottom) {
                    result = 5;
                    if (selfX == itemX) { result = 1; break; }
                    if (selfBottom == itemBottom) result = 4;
                    break;
                }
            }
        }
        if (mask & 8) {
            if (_Z20IsWithinRect0204ea28iiiiss(selfRightM1, selfBottomM1, itemX, itemY, itemRight, itemBottom)) {
                if (selfRight != itemRight || selfBottom != itemBottom) {
                    result = 5;
                    if (selfRight == itemRight) { result = 3; break; }
                    if (selfBottom == itemBottom) result = 4;
                    break;
                }
            }
        }
    }
    return result;
}
