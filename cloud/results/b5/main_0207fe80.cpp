#include <globaldefs.h>

struct Sub0207fe80 {
    char pad0[0x1c];
    unsigned char lo : 4;
    unsigned char hi : 4;
    char pad1[0x20 - 0x1d];
};

struct Elem0207fe80 {
    char pad0[4];
    struct Sub0207fe80* f4;
    char pad1[0xa8 - 8];
    short fa8;
    char pad2[2];
    short fac;
    short fae;
    char pad3[0xc0 - 0xb0];
    short fc0;
    char pad4[0xc4 - 0xc2];
    unsigned char key;
    unsigned char flags;
    char pad5[0xe0 - 0xc6];
};

struct Cont0207fe80 {
    char pad0[0x2c];
    struct Sub0207fe80* subs;
    struct Elem0207fe80* elems;
    char pad1[0x38 - 0x34];
    unsigned char count38;
    unsigned char count39;
};

int CheckField0x9cSetWhenField0xd4Present(unsigned char*);
extern "C" void func_0204bc74(struct Sub0207fe80* s, int mode, short ac, short ae, short a8, short c0, int flag);
extern "C" int func_0204fd00(struct Elem0207fe80* e, int mask);

// USA: func_0207fe80
ARM void ApplyPairedLinks0207fe80(struct Cont0207fe80* obj, int a, int b, unsigned short c) {
    struct Sub0207fe80* sa;
    struct Sub0207fe80* sb;
    struct Sub0207fe80* p;
    struct Elem0207fe80* e;
    unsigned char i;
    unsigned char j;
    short ac;
    short ae;
    short a8;
    short c0;

    a &= 3;
    b &= 3;
    if (a == b) {
        return;
    }
    sa = NULL;
    sb = NULL;
    for (i = 0; i < obj->count38; i++) {
        p = &obj->subs[i];
        if (obj->subs[i].hi == a) {
            sa = p;
        }
        if (obj->subs[i].hi == b) {
            sb = p;
        }
    }
    if ((sa == NULL) | (sb == NULL)) {
        return;
    }
    c &= 0x3ff;
    for (j = 0; j < obj->count39; j++) {
        e = &obj->elems[j];
        if (CheckField0x9cSetWhenField0xd4Present((unsigned char*)e) && e->key != 0xff) {
            if (!(e->flags & 0x20) && !(e->flags & 0x80) && sb == e->f4) {
                ae = e->fae;
                ac = e->fac;
                a8 = e->fa8;
                c0 = e->fc0;
                func_0204bc74(sa, c, ac, ae, a8, c0, 0);
                if (c0 && c) {
                    if (!func_0204fd00(e, 1)) {
                        func_0204bc74(sa, 2, ac, ae, 1, 1, 0);
                    }
                    if (!func_0204fd00(e, 2)) {
                        func_0204bc74(sa, 0x402, (short)(ac + a8 - 1), ae, 1, 1, 0);
                    }
                    if (!func_0204fd00(e, 4)) {
                        func_0204bc74(sa, 0x802, ac, (short)(ae + c0 - 1), 1, 1, 0);
                    }
                    if (!func_0204fd00(e, 8)) {
                        func_0204bc74(sa, 0xc02, (short)(ac + a8 - 1), (short)(ae + c0 - 1), 1, 1, 0);
                    }
                }
            }
        }
    }
}
