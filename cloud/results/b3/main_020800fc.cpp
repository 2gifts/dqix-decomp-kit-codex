#include <globaldefs.h>

struct Ctx0208010c {
    char pad0[4];
    char list4[0x3b - 4];
    signed char f3b;
    signed char f3c;
};

struct Slot0208010c {
    char pad0[8];
    short f8;
    short fa;
    short fc;
    short fe;
    short f10;
};

struct List0207f6ac;

struct Node0208036c {
    char pad0[4];
    short f4;
    char pad1[0x18 - 6];
    struct Node0208036c* f18;
    struct Node0208036c* f1c;
    struct Node0208036c* f20;
    struct Node0208036c* f24;
    char pad2[0x2a - 0x28];
    unsigned char f2a;
};

struct Flags0208036c {
    unsigned int f0;
};

void* FindSlotWithValue020810b4(unsigned char* self, short val);
void SelectCoordsByFlag0x24(unsigned char* obj, int* out1, int* out2);
extern "C" int func_02080e40(void* obj, int p1, int p2, int p3);
struct Node0208036c* FindEntryByShortId0207f6ac(struct List0207f6ac* list, int id);
struct Node0208036c* WalkListByMode0208036c(void* unused, struct Node0208036c* start, int mode, struct Flags0208036c* flags, unsigned char setFlag);

extern unsigned char data_02114e54[];

// USA: func_020800fc
extern "C" ARM int func_020800fc(struct Ctx0208010c* self, short* outId, short id, int mode, unsigned char sel, unsigned char setFlag) {
    struct Flags0208036c flags;
    struct Node0208036c* node;
    struct Node0208036c* e;
    struct Slot0208010c* slot;
    int x;
    int y;
    int ret;

    flags.f0 = 0;
    *outId = id;
    if (data_02114e54[0x55] != 0) {
        slot = (struct Slot0208010c*)FindSlotWithValue020810b4((unsigned char*)self, id);
        if (slot != NULL) {
            SelectCoordsByFlag0x24(data_02114e54, &x, &y);
            ret = func_02080e40(self, slot->f8, (short)x, (short)y);
            if (ret >= 0) {
                *outId = (short)ret;
            }
            if (setFlag != 0) {
                short bs = (short)(slot->fa + slot->fe);
                short xs = (short)(slot->fc + slot->f10);
                if ((short)(xs << 3) - self->f3c <= y && y < (short)(xs << 3)) {
                    short za = (short)(slot->fa << 3);
                    if (za <= x && x < za + self->f3b) {
                        flags.f0 |= 0x20;
                    }
                    if ((short)(bs << 3) - self->f3b <= x && x < (short)(bs << 3)) {
                        flags.f0 |= 0x10;
                    }
                }
            }
        }
        return flags.f0;
    }

    if (sel == 0) {
        return flags.f0;
    }
    e = FindEntryByShortId0207f6ac((struct List0207f6ac*)self->list4, *outId);
    if (e == NULL) {
        return flags.f0;
    }
    node = NULL;
    switch (mode) {
    case 0x10:
        node = e->f24;
        flags.f0 |= 8;
        break;
    case 0x20:
        node = e->f20;
        flags.f0 |= 4;
        break;
    case 0x40:
        node = e->f18;
        flags.f0 |= 1;
        break;
    case 0x80:
        node = e->f1c;
        flags.f0 |= 2;
        break;
    }
    if (node != NULL) {
        if (node->f2a & 2) {
            node = WalkListByMode0208036c(self, e, mode, &flags, setFlag);
        }
        if (node != NULL) {
            *outId = node->f4;
        }
    } else {
        if (sel == 1) {
            e = WalkListByMode0208036c(self, e, mode, &flags, setFlag);
        } else if (sel == 2 && setFlag != 0) {
            switch (mode) {
            case 0x10:
                flags.f0 |= 0x10;
                break;
            case 0x20:
                flags.f0 |= 0x20;
                break;
            }
        }
        *outId = e->f4;
    }
    return flags.f0;
}
