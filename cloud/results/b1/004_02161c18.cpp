#include <globaldefs.h>

struct MenuState02161c18 {
    unsigned char pad0[6];
    signed char field6;
    unsigned char field7;
    unsigned char pad8[3];
    signed char fieldB;
    char* textBuf;
    unsigned char pad10[0xc];
    int combatantId;
};

struct MenuEntry02161c18 {
    short ids[7];
    unsigned char vals[7];
    unsigned char active;
};

struct MenuPage02161c18 {
    struct MenuEntry02161c18 entries[26];
    unsigned char cursor;
    unsigned char pad23d[2];
    unsigned char count;
};

class Node02161c18 {
public:
    virtual void vf00(); virtual void vf04(); virtual void vf08(); virtual void vf0c();
    virtual void vf10(); virtual void vf14(); virtual void vf18(); virtual void vf1c();
    virtual void vf20(); virtual void vf24(); virtual void vf28(); virtual void vf2c();
    virtual void vf30(); virtual void vf34(); virtual void vf38(); virtual void vf3c();
    virtual void vf40(); virtual void vf44(); virtual void vf48(); virtual void vf4c();
    virtual void vf50(); virtual void vf54(); virtual void vf58(); virtual void vf5c();
    virtual void vf60(); virtual void vf64(); virtual void vf68(); virtual void vf6c();
    virtual void vf70(); virtual void vf74(); virtual void vf78(); virtual void vf7c();
    virtual void vf80(); virtual void vf84(); virtual void vf88(); virtual void vf8c();
    virtual void vf90(); virtual void vf94(); virtual void vf98(); virtual void vf9c();
    virtual void vfa0(); virtual void vfa4(); virtual void vfa8(); virtual void vfac();
    virtual void vfb0(); virtual void vfb4(); virtual void vfb8(); virtual void vfbc();
    virtual void vfc0(); virtual void vfc4(); virtual void vfc8(); virtual void vfcc();
    virtual void vfd0(); virtual void vfd4();
    virtual void SetState(int arg);

    unsigned char pad4[8];
    unsigned char flags;
};

extern "C" void __clear(void* buf, int n);
extern "C" void* func_ov011_021849c8(void* self);
extern "C" Node02161c18* func_ov004_0215e47c(void* a, int key);
extern "C" void func_ov004_0215e574(void* a, unsigned short key, int c, int d, int e);
extern "C" void _Z27SetField38IfState8_0215e49cPvis(void* a, int key, int value);
extern "C" Node02161c18* func_ov023_021f6880(void* list, int key);
extern "C" int func_ov023_021f6f10(Node02161c18* node);
extern "C" void _Z36SetNodeFieldsAndMaybeNotify_0215f6f4Pvssi(void* a, short v1, short v2, int flag);

extern struct MenuState02161c18 data_ov004_021707e8;
extern struct MenuPage02161c18 data_ov004_02170b90;
extern int data_ov004_0216fe90[];
extern short data_ov004_02170818[];
extern unsigned char data_ov004_02170814[];

// USA: func_ov004_02161c18
extern "C" ARM int func_ov004_02161c18(void* self) {
    int state;
    int btn;
    void* obj;
    int dim;
    int side;
    int j;
    int i;
    int st;
    int k;
    Node02161c18* node;
    int found;
    int idx;
    int next;
    unsigned char mark[2];
    struct MenuPage02161c18* page;
    struct MenuEntry02161c18* entry;

    data_ov004_021707e8.field7 = 0x66;
    dim = 0;
    obj = func_ov011_021849c8(self);
    if (data_ov004_021707e8.field6 == 0) {
        node = func_ov004_0215e47c(self, 3);
        if (node != 0) node->flags &= ~4;
    } else {
        node = func_ov004_0215e47c(self, 3);
        if (node != 0) node->flags |= 4;
    }

    if (data_ov004_02170b90.count != 0) {
        for (i = 0xd; i < 0xf; i++) _Z27SetField38IfState8_0215e49cPvis(self, i, 0x3a);
        for (i = 0xf; i < 0x1d; i++) _Z27SetField38IfState8_0215e49cPvis(self, i, 0x11e);
        _Z27SetField38IfState8_0215e49cPvis(self, 0x31, 0x3a);
        _Z27SetField38IfState8_0215e49cPvis(self, 0x32, 0x3a);
        for (j = 0; j < 3; j++) {
            node = func_ov023_021f6880(obj, j + 0x65);
            if (node != 0) node->flags &= ~8;
        }
        func_ov004_0215e574(self, 0xc, 0xff, 0, 0);

        page = &data_ov004_02170b90;
        if (page->count <= 1) page->cursor = 0;
        next = page->cursor;
        if (next >= 26) next = 0;
        side = 0;
        __clear(mark, 2);
        found = 0;
        for (i = next; i < 26; i++) {
            idx = data_ov004_0216fe90[i];
            entry = &page->entries[idx - 1];
            if (!entry->active) continue;
            {
                int cid;
                int base;
                _Z27SetField38IfState8_0215e49cPvis(self, side + 0xd, i + 0x19f);
                if (side == 0) {
                    func_ov004_0215e574(self, side + 0xd, 0x80, 0, 0);
                } else {
                    func_ov004_0215e574(self, side + 0xd, 0x80, 0x80, 0);
                }
                node = func_ov023_021f6880(obj, side + 0xd);
                if (node == 0) continue;
                cid = data_ov004_021707e8.combatantId;
                if (idx != data_ov004_02170818[cid] && idx <= 0xe) st = 3;
                else st = 0xf;
                if (idx == 0xd && data_ov004_02170814[cid] == 1) st = 0xf;
                node->SetState(st);
                if (st == 0xf) mark[side] = 1;

                for (k = 0; k < 7; k++) {
                    int key = entry->ids[k];
                    if (key < 0) break;
                    base = 0xf;
                    if (side == 1) base = 0x16;
                    _Z27SetField38IfState8_0215e49cPvis(self, base + k, key);
                    node = func_ov023_021f6880(obj, base + k);
                    if (node != 0 && func_ov023_021f6f10(node) == 8) node->SetState(st);
                }
            }
            next = i + 1;
            found = 1;
            if (++side == 2) break;
        }
        if (found) {
            int avail = 0;
            for (; next < 26; next++) {
                if (page->entries[data_ov004_0216fe90[next] - 1].active) {
                    avail = 1;
                    break;
                }
            }
            if (!avail) next = 26;
            page->cursor = next;
        }
        if (mark[0] == 0 && mark[1] == 0) dim = 1;
    } else {
        for (i = 0xd; i < 0xf; i++) _Z27SetField38IfState8_0215e49cPvis(self, i, 0x3a);
        for (i = 0xf; i < 0x1d; i++) _Z27SetField38IfState8_0215e49cPvis(self, i, 0x11e);
        for (j = 0; j < 3; j++) {
            node = func_ov023_021f6880(obj, j + 0x65);
            if (node != 0) node->flags |= 8;
        }
        _Z27SetField38IfState8_0215e49cPvis(self, 0x31, 0x1e2);
        _Z27SetField38IfState8_0215e49cPvis(self, 0x32, 0x1e3);
        func_ov004_0215e574(self, 0x31, 0xff, 0, 0);
        func_ov004_0215e574(self, 0x32, 0xff, 0, 0);
    }

    state = 0xf;
    if (dim) state = 3;
    node = func_ov023_021f6880(obj, 0xc);
    if (node != 0 && func_ov023_021f6f10(node) == 8) node->SetState(state);
    for (btn = 0; btn < 4; btn++) {
        node = func_ov023_021f6880(obj, btn + 0x64);
        if (node != 0 && func_ov023_021f6f10(node) == 0x13) node->SetState(state);
    }
    _Z36SetNodeFieldsAndMaybeNotify_0215f6f4Pvssi(self, data_ov004_021707e8.fieldB, data_ov004_021707e8.field6, dim);
    return 0;
}
