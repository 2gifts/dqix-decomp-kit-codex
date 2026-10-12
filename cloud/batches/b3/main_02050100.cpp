#include <globaldefs.h>
#include "System/Cache.h"
#include <std_library_functions.h>

extern "C" int func_02050064(void* obj);
extern "C" void func_0204f41c(void* obj, short a, short b, int* c, int d, int e, short* f, short* g, int h);
extern "C" void func_0204f914(void* obj, int flag, short a, short b, short c, short d);
extern "C" void func_0204f7e8(void* obj, short a, short b, int c, int d, int e, short* f, short* g, int h, unsigned char i, int j, int k);
int FindEntryByKey0204254c(int a, int b);
int GetTableEntry0204a5e4(int a, int b);
extern int data_020e7bda;

struct FlagsRef02050100 {
    char pad[0x1c];
    unsigned char nib_lo : 4;
    unsigned char nib_hi : 4;
};

struct PosHalf02050100 {
    short v : 13;
    short w : 3;
};

struct StateBits02050100 {
    unsigned char enabled : 1;
    unsigned char dirty : 1;
};

struct Obj02050100 {
    char pad0[0x4];
    FlagsRef02050100* ref;      // 0x4
    void* buf;                  // 0x8
    char pad1[0xa0-0xc];
    int srcOff;                 // 0xa0
    int len;                    // 0xa4
    PosHalf02050100 q;          // 0xa8
    PosHalf02050100 p;          // 0xaa
    char pad2[0xc5-0xac];
    unsigned char flags2;       // 0xc5
    char pad3[0xd8-0xc6];
    StateBits02050100 flags;    // 0xd8
    unsigned char d9;           // 0xd9
    unsigned char da;           // 0xda
};

// USA: func_02050100
extern "C" ARM void func_02050100(Obj02050100* obj, int flag) {
    if (!obj->flags.enabled)
        return;
    if (!obj->flags.dirty && flag)
        return;
    if (obj->ref == NULL || obj->buf == NULL)
        return;
    unsigned char lo = obj->ref->nib_lo;
    unsigned char hi = obj->ref->nib_hi;
    if (flag) {
        int r3 = func_02050064(obj);
        if (!r3)
            return;
        if (r3) {
            memcpy(obj->buf, (void*)(r3 + obj->srcOff), obj->len);
        }
    }
    short y = obj->p.v * 8 - 13;
    short xs = (obj->q.v * 8) / 2;
    if (obj->flags2 & 0x10)
        y -= 3;
    short ax = 0;
    short ay = 0;
    int cnt = 2;
    signed char n = (signed char)obj->da;
    while (n != 0)
        n = (signed char)((signed char)(n + 1) / 10);
    if (n > 2)
        cnt = n;
    signed char e = 0;
    int r6 = 1;
    if (obj->flags2 & 4)
        r6 = 0;
    int ent = FindEntryByKey0204254c((int)&data_020e7bda, 0);
    if (ent)
        e = *(signed char*)(ent + 4);
    func_0204f41c(obj, (short)(xs - (e >> 1)), y, &data_020e7bda, 8, 0xf, &ax, &ay, 0);
    short X = xs - e;
    func_0204f914(obj, r6, (short)(X - 14), y, X, (short)(y + 9));
    func_0204f7e8(obj, X, y, obj->d9 + 1, 8, 0xf, &ax, &ay, 1, (unsigned char)cnt, 0, 0);
    xs = xs + e;
    func_0204f914(obj, r6, xs, y, (short)(xs + 14), (short)(y + 9));
    func_0204f7e8(obj, (short)(xs + 1), y, obj->da, 8, 0xf, &ax, &ay, 0, (unsigned char)cnt, 0, 0);
    if (flag) {
        void (*fp)(void*, int, unsigned int) = (void (*)(void*, int, unsigned int))GetTableEntry0204a5e4(lo, hi);
        CleanInvalidateCacheRange(obj->buf, obj->len);
        fp(obj->buf, obj->srcOff, obj->len);
    }
    obj->flags.dirty = 0;
}
