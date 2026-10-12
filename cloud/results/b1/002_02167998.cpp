#include <globaldefs.h>
#include "GameState/GameState.h"

struct ShortArrays0204c610;
struct Struct_0205def8;
struct Struct0205de24;
struct Struct_0205d81c;
struct Struct_0205c570;
struct ShortArrays0204c654;
struct Obj0205eaa0;

struct Grid_02167998 {
    char pad0[0x18];
    int row;
    int col;
};

struct Panel_02167998 {
    char pad0[0x54];
    Grid_02167998 grid;
    char pad74[0xb8 - 0x74];
    unsigned char returnMode;
};

struct Stats_02167998 {
    char pad0[0x3c];
    unsigned int : 30;
    unsigned int toggle : 1;
    unsigned int : 1;
};

struct Ext_02167998 {
    char pad0[0x56a];
    unsigned char slot;
    char pad56b[0x94c - 0x56b];
    int kind;
};

struct Touch_02167998 {
    char pad0[0x55];
    unsigned char released;
};

struct Screen_02167998 {
    char pad0[0xc00];
    Panel_02167998 panel;
};

struct Menu_02167998 {
    char pad0[0x2c8];
    Screen_02167998 screen;
    char padf84[0x1a68 - 0xf84];
    int listA;
    char pad1a6c[0x1bb8 - 0x1a6c];
    int nextMode;
    int nextB;
    int phase;
    char pad1bc4[0x1bd8 - 0x1bc4];
    int listB;
    char pad1bdc[0x1c02 - 0x1bdc];
    short cursor;
    short kind;
    short slot;
    char pad1c08[0x1c20 - 0x1c08];
    signed char target;
    char pad1c21[0x1c34 - 0x1c21];
    unsigned char changed;
    char pad1c35[0x1c6e - 0x1c35];
    signed char members[5];
    signed char count;
    char pad1c74[0x247c - 0x1c74];
    unsigned int flags;
};

extern "C" void _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii(Struct_0205def8 *p, int clear, int key);
int GetField0x3acValue(GameState *gs);
extern "C" void _Z32FindAndLinkMatchingEntry0205de24P14Struct0205de24hh(Struct0205de24 *p, unsigned char keyLow,
                                                                        unsigned char keyHigh);
extern "C" void func_ov002_02161cf0(Menu_02167998 *self);
extern "C" void func_ov002_0215f6a4(Menu_02167998 *self);
extern "C" void func_ov002_02161a8c();
extern "C" void func_ov002_021619b4(int id, int a, int b);
extern "C" char *_Z23FindElementByC40205d81cP15Struct_0205d81ci(Struct_0205d81c *p, int key);
void GetFourShortsAtIndex(ShortArrays0204c610 *base, int index, short *out0, short *out1, short *out2, short *out3);
void SetFourShortsAtIndex(ShortArrays0204c654 *base, int index, short v0, short v1, short v2, short v3);
extern "C" int _Z26GetActiveScaledSum0205d794P15Struct_0205c570(Struct_0205c570 *p);
extern "C" void func_0205d7a0(unsigned char *p, int val);
int TestFlag0SetAndFlag1Clear(unsigned short *pad, int buttons);
extern "C" int _Z31IsActiveElementFlag2Set0205da38P12Obj_0205da38(void *p, int key);
void DispatchWithShortB4_0205eaa0(Obj0205eaa0 *obj, int a, int b);
extern "C" void func_ov002_02156e90(Menu_02167998 *self);
extern "C" void func_ov002_0215b9a4(Menu_02167998 *self, int mode, int flag);
Ext_02167998 *GetFieldAt0x150(unsigned char *p);
int CheckFlagOrThreshold_02161b48(char *self, int flag);

extern unsigned char data_ov002_0216cc98[];
extern Touch_02167998 data_02114e54;
extern unsigned short data_02114e30;
extern Obj0205eaa0 data_02108760;

// USA: func_ov002_02167998
extern "C" ARM void func_ov002_02167998(Menu_02167998 *self) {
    Panel_02167998 *panel = &self->screen.panel;
    GameState *gs         = GameState::GetInstance();
    int phase             = self->phase;
    if (phase == 0) {
        self->changed = 0;
        for (int i = 0; i < 5; i++) {
            _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii((Struct_0205def8 *) panel, 0, data_ov002_0216cc98[i]);
        }
        self->target = GetField0x3acValue(gs);
        self->cursor = 0;
        _Z32FindAndLinkMatchingEntry0205de24P14Struct0205de24hh((Struct0205de24 *) panel, 0, 2);
        func_ov002_02161cf0(self);
        func_ov002_0215f6a4(self);
        self->phase++;
        func_ov002_02161a8c();
        func_ov002_021619b4(self->target, self->listA, self->listB);
    } else if (phase == 1) {
        self->flags |= 4;
        char *e = _Z23FindElementByC40205d81cP15Struct_0205d81ci((Struct_0205d81c *) panel, 0x18);
        if (e != 0) {
            short idx = self->count;
            short a, b, c, d;
            GetFourShortsAtIndex((ShortArrays0204c610 *) e, idx, &a, &b, &c, &d);
            idx += self->count + 1;
            SetFourShortsAtIndex((ShortArrays0204c654 *) e, idx, a, b, c, d);
            idx += self->count + 1;
            SetFourShortsAtIndex((ShortArrays0204c654 *) e, idx, a, b, c, d);
        }
        short old    = self->cursor;
        int x        = old % (self->count + 1);
        Grid_02167998 *grid = &panel->grid;
        int row             = grid->row;
        int col             = grid->col;
        self->cursor = _Z26GetActiveScaledSum0205d794P15Struct_0205c570((Struct_0205c570 *) panel);
        if (old != self->cursor) {
            if (x == col && col == self->count && data_02114e54.released == 0) {
                col--;
                short pos = row * (self->count + 1) + col;
                func_0205d7a0((unsigned char *) panel, pos);
                self->cursor = pos;
            }
            self->target = -1;
            if (col < self->count) {
                self->target = self->members[col];
            }
            self->changed = 1;
        }
        int last = col == self->count;
        int more = last == 0;
        _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii((Struct_0205def8 *) panel, more, 0x1a);
        _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii((Struct_0205def8 *) panel, more, 0x1b);
        if (old != self->cursor) return;
        int pressed = TestFlag0SetAndFlag1Clear(&data_02114e30, 0x601);
        int ok      = (pressed | _Z31IsActiveElementFlag2Set0205da38P12Obj_0205da38(&self->screen.panel, 0x14)) ? 1 : 0;
        if (ok) {
            if (col == self->count) {
                DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
                for (int i = 0; i < 5; i++) {
                    _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii((Struct_0205def8 *) panel, 1, data_ov002_0216cc98[i]);
                }
                self->cursor = -1;
                func_ov002_02156e90(self);
                func_ov002_02156e90(self);
                func_ov002_02156e90(self);
                return;
            }
            int id             = self->target;
            GameObject *member = gs->GetPartyMemberByIndex(id);
            if (member == 0) return;
            switch (row) {
                case 0:
                    if (id != GetField0x3acValue(gs)) {
                        func_ov002_0215b9a4(self, self->nextMode & 0xff, 1);
                        DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
                        self->nextMode = 0x19;
                        self->phase    = 0;
                        self->kind     = (signed char) GetFieldAt0x150((unsigned char *) member)->kind;
                    }
                    break;
                case 1: {
                    Stats_02167998 *s = (Stats_02167998 *) member->baseStats_;
                    unsigned short toggle = s->toggle;
                    toggle++;
                    toggle &= 1;
                    s->toggle = toggle;
                    func_ov002_0215b9a4(self, self->nextMode & 0xff, 0);
                    DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
                    break;
                }
                case 2: {
                    Ext_02167998 *ext = GetFieldAt0x150((unsigned char *) member);
                    panel->returnMode = self->nextMode;
                    self->flags &= ~4;
                    DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
                    self->nextMode = 0x1c;
                    self->phase    = 0;
                    self->slot     = ext->slot;
                    break;
                }
            }
        } else if (CheckFlagOrThreshold_02161b48((char *) self, 1)) {
            for (int i = 0; i < 5; i++) {
                _Z31SetElementFlag0x20ByKey0205def8P15Struct_0205def8ii((Struct_0205def8 *) panel, 1, data_ov002_0216cc98[i]);
            }
            self->cursor = -1;
            func_ov002_02156e90(self);
            func_ov002_02156e90(self);
            func_ov002_02156e90(self);
        }
    }
}
