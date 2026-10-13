#include <globaldefs.h>

struct Variant02030b0c;
extern "C" int _ZNK6Script9Parameter5ToIntEv(struct Variant02030b0c* p);
extern "C" void* _ZNK6Script9Parameter8ToStringEv(struct Variant02030b0c* p);
extern "C" void* memcpy(void* dst, const void* src, unsigned int n);
struct List020999f0;
struct Elem020999f0;
struct Elem020995f8 {
    unsigned short id;
    unsigned short wide : 15;
    unsigned short : 1;
    unsigned char flag4 : 1;
    unsigned char : 7;
    unsigned char text[7];
    unsigned char kindA : 4;
    unsigned char flagC4 : 1;
    unsigned char kindC5 : 2;
    unsigned char flagC7 : 1;
    unsigned char valD : 7;
    unsigned char flagD7 : 1;
    unsigned char kindE : 2;
    unsigned char mid : 4;
    unsigned char flagE6 : 1;
    unsigned char flagE7 : 1;
};
void AppendElement020999f0(struct List020999f0*, struct Elem020999f0*);
extern struct List020999f0* data_02109924;

// USA: func_020995f8
extern "C" ARM int TODO_Name_020995f8(struct Variant02030b0c* p) {
    struct Elem020995f8 e;
    void* str;

    e.id = 0;
    e.wide = 0;
    e.text[0] = 0;
    e.kindA = 0;
    e.flagC4 = 0;
    e.kindC5 = 0;
    e.flagC7 = 0;
    e.flagD7 = 0;
    e.kindE = 0;
    e.mid = 8;
    e.flagE7 = 0;
    e.flag4 = 0;

    e.id = _ZNK6Script9Parameter5ToIntEv(p);
    e.wide = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x8));
    _ZNK6Script9Parameter8ToStringEv((struct Variant02030b0c*)((char*)p + 0x10));
    e.kindA = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x18));
    str = _ZNK6Script9Parameter8ToStringEv((struct Variant02030b0c*)((char*)p + 0x20));
    _ZNK6Script9Parameter8ToStringEv((struct Variant02030b0c*)((char*)p + 0x28));
    e.valD = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x30));
    _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x38));
    _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x40));
    e.flagC4 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x48));
    e.kindC5 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x50));
    _ZNK6Script9Parameter8ToStringEv((struct Variant02030b0c*)((char*)p + 0x58));
    e.flagC7 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x60));
    e.flagD7 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x80));
    e.kindE = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x88));
    e.mid = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x90));
    e.flagE6 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0x98));
    e.flagE7 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0xa0));
    e.flag4 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)((char*)p + 0xa8));

    if (str == 0) {
        return 0;
    }
    memcpy(e.text, str, 7);
    e.text[6] = 0;
    AppendElement020999f0(data_02109924, (struct Elem020999f0*)&e);
    return 1;
}
