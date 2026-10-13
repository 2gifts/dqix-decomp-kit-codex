#include <globaldefs.h>
#include <System/Graphics.h>

struct BgControl {
    unsigned short priority : 2;
    unsigned short charBase : 4;
    unsigned short mosaic : 1;
    unsigned short colorMode : 1;
    unsigned short screenBase : 5;
    unsigned short wrap : 1;
    unsigned short screenSize : 2;
};

struct BgSlot {
    unsigned short unused;
    struct BgControl cnt;
};

struct Struct02074bf4 {
    struct BgSlot bg[4];
    unsigned char pad10;
    unsigned char flag11;
};

// USA: func_02074e54
extern "C" ARM void func_02074e54(struct Struct02074bf4* obj) {
    if (obj->flag11) {
        BG0CNTSUB = (obj->bg[0].cnt.screenSize << 14) | (BG0CNTSUB & 0x43) | (obj->bg[0].cnt.colorMode << 7) |
                    (obj->bg[0].cnt.screenBase << 8) | (obj->bg[0].cnt.charBase << 2) | (obj->bg[0].cnt.wrap << 13);
        BG1CNTSUB = (obj->bg[1].cnt.screenSize << 14) | (BG1CNTSUB & 0x43) | (obj->bg[1].cnt.colorMode << 7) |
                    (obj->bg[1].cnt.screenBase << 8) | (obj->bg[1].cnt.charBase << 2) | (obj->bg[1].cnt.wrap << 13);
        BG2CNTSUB = (obj->bg[2].cnt.screenSize << 14) | (BG2CNTSUB & 0x43) | (obj->bg[2].cnt.colorMode << 7) |
                    (obj->bg[2].cnt.screenBase << 8) | (obj->bg[2].cnt.charBase << 2);
        BG3CNTSUB = (obj->bg[3].cnt.screenSize << 14) | (BG3CNTSUB & 0x43) | (obj->bg[3].cnt.colorMode << 7) |
                    (obj->bg[3].cnt.screenBase << 8) | (obj->bg[3].cnt.charBase << 2);

        BG0CNTSUB = (BG0CNTSUB & ~3) | obj->bg[0].cnt.priority;
        BG1CNTSUB = (BG1CNTSUB & ~3) | obj->bg[1].cnt.priority;
        BG2CNTSUB = (BG2CNTSUB & ~3) | obj->bg[2].cnt.priority;
        BG3CNTSUB = (BG3CNTSUB & ~3) | obj->bg[3].cnt.priority;

        if (obj->bg[0].cnt.mosaic) {
            BG0CNTSUB |= 0x40;
        } else {
            BG0CNTSUB &= ~0x40;
        }
        if (obj->bg[1].cnt.mosaic) {
            BG1CNTSUB |= 0x40;
        } else {
            BG1CNTSUB &= ~0x40;
        }
        if (obj->bg[2].cnt.mosaic) {
            BG2CNTSUB |= 0x40;
        } else {
            BG2CNTSUB &= ~0x40;
        }
        if (obj->bg[3].cnt.mosaic) {
            BG3CNTSUB |= 0x40;
        } else {
            BG3CNTSUB &= ~0x40;
        }
    }
}
