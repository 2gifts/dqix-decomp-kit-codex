#include <globaldefs.h>

struct BgCnt {
    unsigned short priority : 2;
    unsigned short charBase : 4;
    unsigned short mosaic : 1;
    unsigned short colorMode : 1;
    unsigned short screenBase : 5;
    unsigned short extPalette : 1;
    unsigned short screenSize : 2;
};

struct BgSlot {
    struct BgCnt cnt;
    unsigned short pad;
};

struct Struct02074c18 {
    struct BgSlot bg[4];
    unsigned char flag10;
};

// USA: func_02074c18
ARM void ApplyBgCnt02074c18(struct Struct02074c18* obj) {
    if (obj->flag10) {
        volatile unsigned short* reg = (volatile unsigned short*)0x04000008;

        reg[0] = (obj->bg[0].cnt.screenSize << 14) | (reg[0] & 0x43) | (obj->bg[0].cnt.colorMode << 7) |
                 (obj->bg[0].cnt.screenBase << 8) | (obj->bg[0].cnt.charBase << 2) | (obj->bg[0].cnt.extPalette << 13);
        reg[1] = (obj->bg[1].cnt.screenSize << 14) | (reg[1] & 0x43) | (obj->bg[1].cnt.colorMode << 7) |
                 (obj->bg[1].cnt.screenBase << 8) | (obj->bg[1].cnt.charBase << 2) | (obj->bg[1].cnt.extPalette << 13);
        reg[2] = (obj->bg[2].cnt.screenSize << 14) | (reg[2] & 0x43) | (obj->bg[2].cnt.colorMode << 7) |
                 (obj->bg[2].cnt.screenBase << 8) | (obj->bg[2].cnt.charBase << 2);
        reg[3] = (obj->bg[3].cnt.screenSize << 14) | (reg[3] & 0x43) | (obj->bg[3].cnt.colorMode << 7) |
                 (obj->bg[3].cnt.screenBase << 8) | (obj->bg[3].cnt.charBase << 2);

        reg[0] = (reg[0] & ~3) | obj->bg[0].cnt.priority;
        reg[1] = (reg[1] & ~3) | obj->bg[1].cnt.priority;
        reg[2] = (reg[2] & ~3) | obj->bg[2].cnt.priority;
        reg[3] = (reg[3] & ~3) | obj->bg[3].cnt.priority;

        if (obj->bg[0].cnt.mosaic) {
            reg[0] |= 0x40;
        } else {
            reg[0] &= ~0x40;
        }
        {
            volatile unsigned short* bg1 = (volatile unsigned short*)0x0400000a;
            if (obj->bg[1].cnt.mosaic) {
                *bg1 |= 0x40;
            } else {
                *bg1 &= ~0x40;
            }
        }
        {
            volatile unsigned short* bg2 = (volatile unsigned short*)0x0400000c;
            if (obj->bg[2].cnt.mosaic) {
                *bg2 |= 0x40;
            } else {
                *bg2 &= ~0x40;
            }
        }
        {
            volatile unsigned short* bg3 = (volatile unsigned short*)0x0400000e;
            if (obj->bg[3].cnt.mosaic) {
                *bg3 |= 0x40;
            } else {
                *bg3 &= ~0x40;
            }
        }
    }
}
