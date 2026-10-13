#include <globaldefs.h>
#include "std_library_functions.h"

struct State0204166c {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    unsigned short f14;
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char pad16 : 4;
    unsigned char f17;
    int f18;
};

struct PtrTarget020459b8 {
    char unk0[0x50];
    int w50;
};

struct CombatEntries020459b8 {
    char unk0[0x2cc];
    unsigned char bits2cc[4];
    char unk2d0[0x9b8 - 0x2d0];
    struct State0204166c entries[0x80];
    unsigned char active[0x80];
    char unk1838[0x195b - 0x1838];
    unsigned char flags195b;
    char unk195c[0x19cc - 0x195c];
    unsigned char byte19cc;
    char unk19cd[0x1e28 - 0x19cd];
    struct PtrTarget020459b8* ptrField;
};

extern char data_02107800;
extern unsigned char data_020e7a8c[];
extern const short data_020e7aa6[];
extern short data_020e7ab0[];

int FindAndSetFirstFreeBit(int, unsigned char*, unsigned int);
void InitState0204166c020416e0(struct State0204166c*, unsigned char, int, int, int, int, unsigned char);
extern "C" void func_02045864(short, short, int, int, int, int, unsigned char, int);

// USA: func_020459b8
ARM int AllocateCombatEntry020459b8(struct CombatEntries020459b8* obj, unsigned char arg1, int arg2, unsigned char arg3) {
    int i;
    unsigned char* active;
    struct State0204166c* entries;
    char* base;
    char* p;
    int idx;
    int flag18;
    int flag14;

    if (arg2 == 0) return 0;
    active = obj->active;
    entries = obj->entries;
    base = *(char**)&data_02107800;
    idx = FindAndSetFirstFreeBit((int)obj, obj->bits2cc, 4);
    if (idx < 0) return 0;

    for (i = 0; i < 0x80; i++) {
        struct State0204166c* e;
        if (active[i] != 0) continue;
        if (obj->ptrField != NULL) {
            flag18 = 0;
            flag14 = 0;
            e = entries + i;
            base += idx * 0x80;
            p = base;
            if (obj->byte19cc == 0) {
                memset(p, flag18, 0x80);
                if (obj->flags195b & 0x80) {
                    flag14 = 1;
                    for (unsigned char j = 0; j < 5; j++) {
                        unsigned char kind = data_020e7a8c[j];
                        func_02045864(data_020e7aa6[j], data_020e7ab0[j], arg2, (int)p, 0x10, 0x10, kind, arg3);
                    }
                } else if (obj->flags195b & 2) {
                    flag18 = 1;
                    func_02045864(2, 2, arg2, (int)p, 0x10, 0x10, 1, arg3);
                    func_02045864(1, 1, arg2, (int)p, 0x10, 0x10, 0xf, arg3);
                } else {
                    func_02045864(1, 1, arg2, (int)p, 0x10, 0x10, 0xf, arg3);
                }
            }
            InitState0204166c020416e0(e, arg1, arg2, (int)p, idx, (unsigned short)obj->ptrField->w50 * 8 + i * 0x80, arg3);
            if (flag14) e->flag2 = 1;
            if (flag18) e->flag1 = 1;
        }
        active[i] = 1;
        return i;
    }
    return 0;
}
