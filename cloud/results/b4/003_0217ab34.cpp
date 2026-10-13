#include <globaldefs.h>
#include "GameState/GameState.h"

struct Obj2081;
struct Cont0207fdf0;
struct Obj0205eaa0;
struct Obj0208203c;
struct Container020dedd0;
struct Obj021dcae0;
struct Obj02179e18;

struct Obj02179e18 {
    char pad[8];
    unsigned int unk_ : 12;
    unsigned int kind_ : 2;
};

struct Table0217ab34 {
    char pad[0xc];
    short vals[1];
};

struct Menu_0217ab34 {
    char unk_0[8];
    struct Obj02179e18* elem;
    char unk_c[0x30];
    char sub3c[0x874 - 0x3c];
    char container[0x88c - 0x874];
    char repeat[0x89c - 0x88c];
    struct Obj2081* menu;
    char unk_8a0[0xff8 - 0x8a0];
    short* cursor;
    short prevKey;
    short key;
    char unk_1000[0xe];
    short member100e;
    char unk_1010[4];
    short member1014;
    char unk_1016[0x1036 - 0x1016];
    short nextState;
    char unk_1038[2];
    short sel;
    unsigned char count;
    char unk_103d;
    unsigned char mode;
    unsigned char step;
    char unk_1040[6];
    unsigned short flags;
    char unk_1048[2];
    unsigned char tick;
    unsigned char up;
    unsigned char down;
};

void* GetPtrField0x2a04(GameState* gs);
short FindMappedMemberId02080468(void* obj, int id);
void SetElementFlag0x40(struct Obj2081* obj, int id, int value);
void SetEntryLowNibbleAndElement02080c68(void* obj, int id, int value);
void ResetWithSub0208203c(struct Obj0208203c* obj);
void DispatchWithShortB4_0205eaa0(struct Obj0205eaa0* obj, int a, int b);
struct Obj02179e18* FindElementByKey020dedd0(struct Container020dedd0* c, int key);
void CallFunc0204c804OnNonMatchingKey(struct Cont0207fdf0* obj, int key);
void CallFunc0204c804OnMatchingKey(struct Obj2081* obj, int key);
unsigned short GetInnerShort_0217b200(void** obj);
int SumKeyedLookups02086bf4(char* table, int key);
int ComputeTimeRemaining_02179e18(struct Obj02179e18* elem);
int TestFlag0SetAndFlag1Clear(unsigned short* obj, int mask);
void RefillAndResetEntry_02175d78(char* obj);

extern "C" int func_ov003_02179f34(void* self);
extern "C" void func_ov003_02177dd4(void* self);
extern "C" void func_ov003_021748c4(void* self);
extern "C" void func_ov003_0217a14c(void* self);
extern "C" void func_ov023_021dcae0(struct Obj021dcae0* obj, int val);
extern "C" int func_ov003_021765b4(void* self);
extern "C" int func_ov003_02179da8(void* elem, int flag);
extern "C" int func_ov003_021766e8(void* self);
extern "C" void func_ov003_02177208(void* self);
extern "C" void func_ov003_0217726c(void* self);
extern "C" int func_ov003_02178218(void* self);
extern "C" int func_02081f20(void* obj, unsigned int tick);
extern "C" int func_ov003_021782a0(void* self);
extern "C" void func_ov003_02176468(void* self);
extern "C" int func_ov003_021764f4(void* self);
extern "C" void func_ov003_02175df4(void* self);

extern struct Obj0205eaa0 data_02108760;
extern unsigned short data_02114e30;

// USA: func_ov003_0217ab34
extern "C" ARM void func_ov003_0217ab34(struct Menu_0217ab34* self) {
    char* data = (char*)GetPtrField0x2a04(GameState::GetInstance());
    struct Obj2081* menu = self->menu;

    if (self->step == 0) {
        int tier = func_ov003_02179f34(self);
        self->flags &= ~0x400;
        if (tier > 8) {
            self->flags |= 0x400;
        }
        self->key = 0x16;
        if (self->member100e < 0) {
            self->member100e = FindMappedMemberId02080468(menu, self->key);
        }
        short base = FindMappedMemberId02080468(menu, self->key);
        short idx = (short)(self->member100e - base);
        struct Table0217ab34* table = (struct Table0217ab34*)data;
        self->sel = table->vals[(short)(self->member1014 * 8 + idx)];
        self->count = 1;
        func_ov003_02177dd4(self);
        SetElementFlag0x40(menu, self->key, 0);
        SetEntryLowNibbleAndElement02080c68(menu, self->key, 0);
        ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
        self->cursor = 0;
        self->step = 1;
        func_ov003_021748c4(self);
        return;
    }

    if (self->step == 1) {
        short prevSel = self->sel;
        self->cursor = &self->member100e;
        func_ov003_0217a14c(self);
        func_ov023_021dcae0((struct Obj021dcae0*)self->sub3c, self->sel);
        if (func_ov003_021765b4(self) && prevSel == self->sel) {
            DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
            ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
            self->cursor = 0;
            self->elem = FindElementByKey020dedd0((struct Container020dedd0*)self->container, self->sel);
            if (self->elem == 0) {
                return;
            }
            self->prevKey = self->key;
            SetEntryLowNibbleAndElement02080c68(menu, self->key, 1);
            self->step = 2;
            if (self->elem->kind_ == 0) {
                CallFunc0204c804OnNonMatchingKey((struct Cont0207fdf0*)menu, 0);
                self->nextState = 0x12;
                self->step = 0;
                self->flags |= 0x20;
                self->flags &= ~0x2000;
                return;
            }
            switch (func_ov003_02179da8(self->elem, 1)) {
            case 1:
                CallFunc0204c804OnNonMatchingKey((struct Cont0207fdf0*)menu, 0);
                self->nextState = 0xd;
                self->step = 6;
                self->flags |= 0x2000;
                if (self->elem->kind_ == 2) {
                    self->nextState = 0x11;
                    self->step = 4;
                }
                break;
            case 2:
                CallFunc0204c804OnNonMatchingKey((struct Cont0207fdf0*)menu, 0);
                self->nextState = 0x1c;
                self->mode = 1;
                self->step = 0;
                self->flags |= 0x2000;
                break;
            }
            return;
        }
        if (func_ov003_021766e8(self) == 0) {
            return;
        }
        ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
        self->cursor = 0;
        CallFunc0204c804OnNonMatchingKey((struct Cont0207fdf0*)menu, 0);
        self->mode = 4;
        self->step = 1;
        self->flags |= 0x2000;
        return;
    }

    if (self->step == 2) {
        func_ov003_02177208(self);
        func_ov003_0217726c(self);
        ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
        self->step = 3;
        self->tick = 0;
        return;
    }

    if (self->step == 3) {
        int dir = 0;
        if (self->tick < 5) {
            self->tick++;
        } else {
            dir = func_ov003_02178218(self);
            if (dir != 0) {
                self->tick = 0;
            }
        }
        int state = func_02081f20(self->repeat, GameState::GetInstance()->GetTickCount());
        if ((unsigned short)(state + 0xffff) <= 1) {
            unsigned short pad = GetInnerShort_0217b200((void**)self->repeat);
            if (pad == 0x40) {
                dir = 1;
            } else if (pad == 0x80) {
                dir = -1;
            } else if (pad == 0x20) {
                dir = 0x83;
            } else if (pad == 0x10) {
                dir = -0x83;
            }
        }
        if (dir != 0) {
            int next = self->count + dir;
            int limit = SumKeyedLookups02086bf4(data, self->sel);
            int remaining = ComputeTimeRemaining_02179e18(self->elem);
            if (remaining < limit) {
                limit = remaining;
            }
            if (limit < next) {
                next = limit;
            }
            if (next <= 1) {
                next = 1;
            }
            self->up = self->count < next;
            self->down = next < self->count;
            self->count = next;
            func_ov003_02177208(self);
            func_ov003_0217726c(self);
        }
        int pressed = func_ov003_021782a0(self);
        if (TestFlag0SetAndFlag1Clear(&data_02114e30, 0x401) != 0 || pressed == 1) {
            DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
            CallFunc0204c804OnNonMatchingKey((struct Cont0207fdf0*)menu, 0);
            self->nextState = 0xd;
            if (self->count > 1) {
                self->nextState = 0x20;
            }
            self->step = 6;
            self->flags |= 0x2000;
            ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
            if (self->elem->kind_ == 2) {
                self->nextState = 0x11;
                self->step = 4;
            }
        } else if (TestFlag0SetAndFlag1Clear(&data_02114e30, 2) != 0 || pressed == -1) {
            CallFunc0204c804OnMatchingKey(menu, 9);
            CallFunc0204c804OnMatchingKey(menu, 10);
            ResetWithSub0208203c((struct Obj0208203c*)self->repeat);
            self->step = 0;
        }
        return;
    }

    if (self->step == 4) {
        func_ov003_02176468(self);
        self->step = 5;
        return;
    }

    if (self->step == 5) {
        int res = func_ov003_021764f4(self);
        if (res != -1) {
            if (res != 1) {
                return;
            }
            self->nextState = 0x10;
            self->step = 6;
        } else {
            self->nextState = 0x30;
            self->step = 0;
            self->flags |= 0x20;
        }
        return;
    }

    if (self->step == 6) {
        func_ov003_02176468(self);
        self->step = 7;
        return;
    }

    if (self->step == 7) {
        int res = func_ov003_021764f4(self);
        if (res != -1) {
            if (res != 1) {
                return;
            }
            RefillAndResetEntry_02175d78((char*)self);
            func_ov003_02175df4(self);
            self->nextState = 0xe;
            self->mode = 4;
            self->step = 1;
            self->flags |= 0x20;
        } else {
            self->nextState = 0x30;
            self->mode = 6;
            self->step = 0;
            self->flags |= 0x20;
        }
    }
}
