#include <globaldefs.h>

struct BgControl {
    unsigned short priority : 2;
    unsigned short charBase : 4;
    unsigned short mosaic : 1;
    unsigned short colorMode : 1;
    unsigned short screenBase : 5;
    unsigned short wrap : 1;
    unsigned short screenSize : 2;
};

union BgControlReg {
    unsigned short raw;
    struct BgControl bits;
};

struct BgSlot {
    unsigned short unused;
    union BgControlReg cnt;
};

struct Struct02074bf4 {
    struct BgSlot bg[4];
    unsigned char pad10;
    unsigned char flag11;
};

// USA: func_02074e54
extern "C" ARM void func_02074e54(struct Struct02074bf4* obj) {
    if (obj->flag11) {
        volatile union BgControlReg* regs = (volatile union BgControlReg*)0x04001008;

        regs[0].raw = (obj->bg[0].cnt.bits.screenSize << 14) | (regs[0].raw & 0x43) | (obj->bg[0].cnt.bits.colorMode << 7) |
                      (obj->bg[0].cnt.bits.screenBase << 8) | (obj->bg[0].cnt.bits.charBase << 2) | (obj->bg[0].cnt.bits.wrap << 13);
        regs[1].raw = (obj->bg[1].cnt.bits.screenSize << 14) | (regs[1].raw & 0x43) | (obj->bg[1].cnt.bits.colorMode << 7) |
                      (obj->bg[1].cnt.bits.screenBase << 8) | (obj->bg[1].cnt.bits.charBase << 2) | (obj->bg[1].cnt.bits.wrap << 13);
        regs[2].raw = (obj->bg[2].cnt.bits.screenSize << 14) | (regs[2].raw & 0x43) | (obj->bg[2].cnt.bits.colorMode << 7) |
                      (obj->bg[2].cnt.bits.screenBase << 8) | (obj->bg[2].cnt.bits.charBase << 2);
        regs[3].raw = (obj->bg[3].cnt.bits.screenSize << 14) | (regs[3].raw & 0x43) | (obj->bg[3].cnt.bits.colorMode << 7) |
                      (obj->bg[3].cnt.bits.screenBase << 8) | (obj->bg[3].cnt.bits.charBase << 2);

        regs[0].raw = (regs[0].raw & ~3) | obj->bg[0].cnt.bits.priority;
        regs[1].raw = (regs[1].raw & ~3) | obj->bg[1].cnt.bits.priority;
        regs[2].raw = (regs[2].raw & ~3) | obj->bg[2].cnt.bits.priority;
        regs[3].raw = (regs[3].raw & ~3) | obj->bg[3].cnt.bits.priority;

        if (obj->bg[0].cnt.bits.mosaic) {
            regs[0].bits.mosaic = 1;
        } else {
            regs[0].bits.mosaic = 0;
        }
        if (obj->bg[1].cnt.bits.mosaic) {
            ((volatile union BgControlReg*)0x0400100a)->bits.mosaic = 1;
        } else {
            ((volatile union BgControlReg*)0x0400100a)->bits.mosaic = 0;
        }
        if (obj->bg[2].cnt.bits.mosaic) {
            ((volatile union BgControlReg*)0x0400100c)->bits.mosaic = 1;
        } else {
            ((volatile union BgControlReg*)0x0400100c)->bits.mosaic = 0;
        }
        if (obj->bg[3].cnt.bits.mosaic) {
            ((volatile union BgControlReg*)0x0400100e)->bits.mosaic = 1;
        } else {
            ((volatile union BgControlReg*)0x0400100e)->bits.mosaic = 0;
        }
    }
}
