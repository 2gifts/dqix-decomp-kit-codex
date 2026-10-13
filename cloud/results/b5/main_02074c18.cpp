#include <globaldefs.h>
#include "System/Graphics.h"

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
        BG0CNT = (obj->bg[0].cnt.screenSize << 14) | (BG0CNT & 0x43) | (obj->bg[0].cnt.colorMode << 7) |
                 (obj->bg[0].cnt.screenBase << 8) | (obj->bg[0].cnt.charBase << 2) | (obj->bg[0].cnt.extPalette << 13);
        BG1CNT = (obj->bg[1].cnt.screenSize << 14) | (BG1CNT & 0x43) | (obj->bg[1].cnt.colorMode << 7) |
                 (obj->bg[1].cnt.screenBase << 8) | (obj->bg[1].cnt.charBase << 2) | (obj->bg[1].cnt.extPalette << 13);
        BG2CNT = (obj->bg[2].cnt.screenSize << 14) | (BG2CNT & 0x43) | (obj->bg[2].cnt.colorMode << 7) |
                 (obj->bg[2].cnt.screenBase << 8) | (obj->bg[2].cnt.charBase << 2);
        BG3CNT = (obj->bg[3].cnt.screenSize << 14) | (BG3CNT & 0x43) | (obj->bg[3].cnt.colorMode << 7) |
                 (obj->bg[3].cnt.screenBase << 8) | (obj->bg[3].cnt.charBase << 2);

        BG0CNT = (BG0CNT & ~3) | obj->bg[0].cnt.priority;
        BG1CNT = (BG1CNT & ~3) | obj->bg[1].cnt.priority;
        BG2CNT = (BG2CNT & ~3) | obj->bg[2].cnt.priority;
        BG3CNT = (BG3CNT & ~3) | obj->bg[3].cnt.priority;

        if (obj->bg[0].cnt.mosaic) {
            BG0CNT |= 0x40;
        } else {
            BG0CNT &= ~0x40;
        }
        if (obj->bg[1].cnt.mosaic) {
            BG1CNT |= 0x40;
        } else {
            BG1CNT &= ~0x40;
        }
        if (obj->bg[2].cnt.mosaic) {
            BG2CNT |= 0x40;
        } else {
            BG2CNT &= ~0x40;
        }
        if (obj->bg[3].cnt.mosaic) {
            BG3CNT |= 0x40;
        } else {
            BG3CNT &= ~0x40;
        }
    }
}
