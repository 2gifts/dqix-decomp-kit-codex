#include <globaldefs.h>

struct Variant02030b0c {
    int tag;
    int value;
};
extern "C" int _ZNK6Script9Parameter5ToIntEv(struct Variant02030b0c* p);

struct List02071cc8;
struct Elem02071cc8;
void AppendElementCapped02071cc8(struct List02071cc8*, struct Elem02071cc8*);

struct FieldC02071b38 {
    unsigned int lo10 : 10;
    unsigned int mid10 : 10;
    unsigned int hi8 : 8;
    unsigned int top4 : 4;
};

struct Field1002071b38 {
    unsigned int lo10 : 10;
    unsigned int mid10 : 10;
    unsigned int bit20 : 1;
    unsigned int bit21 : 1;
    unsigned int bit22 : 1;
    unsigned int hi8 : 8;
    unsigned int bit31 : 1;
};

struct Struct02071b38 {
    short f0;
    short f2;
    short f4;
    short f6;
    short f8;
    unsigned short fa;
    FieldC02071b38 fc;
    Field1002071b38 f10;
    short f14;
    short f16;
    short f18;
    short f1a;
    int f1c;
};
extern "C" void func_02071b38(struct Struct02071b38* s);

struct Data02108d98 {
    unsigned short field0;
    unsigned short field2;
    short* field4;
    int field8;
    struct List02071cc8* fieldC;
};
extern struct Data02108d98 data_02108d98;

// USA: func_02071874
extern "C" ARM int TODO_Name_02071874(struct Variant02030b0c* variant) {
    struct Variant02030b0c* cur = variant;
    struct Struct02071b38 entry;
    short id = (short)_ZNK6Script9Parameter5ToIntEv(cur);

    if (data_02108d98.field4 != 0) {
        int found = 0;
        unsigned short i;
        for (i = 0; i < data_02108d98.field0; i++) {
            if (data_02108d98.field4[i] == id) {
                found = 1;
                break;
            }
        }
        if (!found) {
            return 1;
        }
    }

    unsigned short v;

    func_02071b38(&entry);
    entry.f0 = id;
    entry.f2 = (short)_ZNK6Script9Parameter5ToIntEv(&cur[1]);
    if (cur[2].tag != 0) {
        struct Variant02030b0c* arg = &cur[2];
        cur += 3;
        entry.f4 = (short)_ZNK6Script9Parameter5ToIntEv(arg);
    } else {
        entry.f4 = 0;
        cur += 3;
    }
    v = (unsigned short)_ZNK6Script9Parameter5ToIntEv(cur);
    entry.fa = (unsigned short)((entry.fa & ~0xf) | (v & 0xf));
    if (cur[1].tag != 0) {
        struct Variant02030b0c* arg = &cur[1];
        cur += 2;
        entry.f6 = (short)_ZNK6Script9Parameter5ToIntEv(arg);
    } else {
        entry.f6 = 0;
        cur += 2;
    }
    v = (unsigned short)_ZNK6Script9Parameter5ToIntEv(cur);
    entry.fa = (unsigned short)((entry.fa & ~0xf0) | ((v & 0xf) << 4));
    if (cur[1].tag != 0) {
        struct Variant02030b0c* arg = &cur[1];
        cur += 2;
        entry.f8 = (short)_ZNK6Script9Parameter5ToIntEv(arg);
    } else {
        entry.f8 = 0;
        cur += 2;
    }
    v = (unsigned short)_ZNK6Script9Parameter5ToIntEv(cur);
    entry.fa = (unsigned short)((entry.fa & ~0xf00) | ((v & 0xf) << 8));
    v = (unsigned short)_ZNK6Script9Parameter5ToIntEv(&cur[1]);
    entry.fa = (unsigned short)((entry.fa & ~0xf000) | ((v & 0xf) << 12));

    entry.fc.lo10 = _ZNK6Script9Parameter5ToIntEv(&cur[2]);
    entry.fc.mid10 = _ZNK6Script9Parameter5ToIntEv(&cur[3]);
    entry.f10.lo10 = _ZNK6Script9Parameter5ToIntEv(&cur[4]);
    entry.f10.mid10 = _ZNK6Script9Parameter5ToIntEv(&cur[5]);
    entry.f10.bit20 = _ZNK6Script9Parameter5ToIntEv(&cur[6]);
    entry.fc.hi8 = _ZNK6Script9Parameter5ToIntEv(&cur[7]);
    entry.f10.hi8 = _ZNK6Script9Parameter5ToIntEv(&cur[8]);
    entry.f14 = (short)_ZNK6Script9Parameter5ToIntEv(&cur[9]);
    entry.f16 = (short)_ZNK6Script9Parameter5ToIntEv(&cur[10]);
    entry.f18 = (short)_ZNK6Script9Parameter5ToIntEv(&cur[11]);
    entry.f1a = (short)_ZNK6Script9Parameter5ToIntEv(&cur[12]);

    AppendElementCapped02071cc8(data_02108d98.fieldC, (struct Elem02071cc8*)&entry);
    return 1;
}
