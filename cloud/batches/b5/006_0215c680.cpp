#include <globaldefs.h>

struct Obj2081;
struct Obj0215951c;
struct Obj0208203c;
struct Obj0205eaa0;
struct Obj02159094;
struct StructC28;
struct Struct021596f0;

void CallFunc0204c804OnMatchingKey(struct Obj2081*, int);
int FindMappedMemberId02080468(void*, int);
void SetElementFlag0x40(struct Obj2081*, int, int);
extern "C" void func_020813ec(struct Obj2081*, int);
void SetEntryLowNibbleAndElement02080c68(void*, int, int);
void CalcField389_0215951c(struct Obj0215951c*);
void ClearSublistEntriesFlag4(void*, int);
void ClearFlagsForIds_021573c8(void*);
void ResetWithSub0208203c(struct Obj0208203c*);
void DispatchWithShortB4_0205eaa0(struct Obj0205eaa0*, int, int);
void ExpireStatusSlot02159094(struct Obj02159094*);
void ClearField44IfSet_02158c28(struct StructC28*);
int Check02159564(void*);
void UpdateEntryFlags021596f0(struct Struct021596f0*);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);

struct Obj0215c680;
extern "C" void func_ov006_0215f4dc(Obj0215c680*);
extern "C" void func_ov006_021553bc(void*, short*, unsigned char*);
extern "C" void func_ov006_02155710(void*, int, int);
extern "C" int func_ov006_021595b4(Obj0215c680*);
extern "C" int func_ov006_02158a28(Obj0215c680*);
extern "C" int func_ov006_02159218(Obj0215c680*);
extern "C" int func_ov006_02158b78(Obj0215c680*);
extern "C" void func_ov006_0215943c(Obj0215c680*);
extern "C" void func_ov006_02159274(Obj0215c680*, unsigned char*, short*, unsigned char*);
extern "C" int func_ov006_02159834(Obj0215c680*);
extern "C" void func_ov006_02159108(Obj0215c680*, int, int, int);
extern "C" void func_ov006_021570fc(void*, int);
extern "C" void func_ov006_02159db0(Obj0215c680*);
extern "C" int func_ov006_02159e50(Obj0215c680*);

extern struct Obj0205eaa0 data_02108760;
extern unsigned short data_02114e30;

struct Obj0215c680 {
    char pad0[0x10];
    void* ctx;
    struct Obj2081* elements;
    char pad18[0x44 - 0x18];
    short* cur;
    char pad48[0x9c - 0x48];
    char sub[0x100 - 0x9c];
    char list[0x35e - 0x100];
    short field35e;
    char pad360[0x362 - 0x360];
    short field362;
    short selected;
    char pad366[0x36e - 0x366];
    short field36e;
    short field370;
    char pad372[0x378 - 0x372];
    short ids[3];
    short message;
    char pad380[0x38a - 0x380];
    unsigned char field38a;
    unsigned char field38b;
    unsigned char mergeBuf[3];
    unsigned char mode;
    unsigned char state;
    char pad391;
    unsigned char messageTimer;
    unsigned char field393;
    unsigned short flags;
    char pad396[0x3b4 - 0x396];
    int busy;
    char pad3b8[0x42c - 0x3b8];
    unsigned char field42c;
};

// USA: func_ov006_0215c680
extern "C" ARM void func_ov006_0215c680(Obj0215c680* self) {
    Obj2081* e = self->elements;
    if (self->state == 0) {
        CallFunc0204c804OnMatchingKey(e, 0);
        CallFunc0204c804OnMatchingKey(e, 1);
        self->field36e = 0x10;
        if (self->field362 < 0) {
            self->field362 = FindMappedMemberId02080468(e, self->field36e);
        }
        SetElementFlag0x40(e, self->field36e, 0);
        func_020813ec(e, self->field36e);
        SetEntryLowNibbleAndElement02080c68(e, self->field36e, 0);
        self->field370 = -1;
        self->field38b = 0;
        CalcField389_0215951c((Obj0215951c*)self);
        SetElementFlag0x40(e, 0x11, 0);
        ClearSublistEntriesFlag4(e, 0x11);
        func_ov006_0215f4dc(self);
        self->flags |= 0x30;
        SetEntryLowNibbleAndElement02080c68(e, 0x11, 1);
        ClearFlagsForIds_021573c8(&self->list[0]);
        func_ov006_021553bc(self->ctx, self->ids, self->mergeBuf);
        ResetWithSub0208203c((Obj0208203c*)self->sub);
        self->cur = 0;
        func_ov006_02155710(self->ctx, -1, 0);
        self->state++;
    } else if (self->state == 1) {
        if (func_ov006_021595b4(self)) {
            return;
        }
        self->cur = &self->field362;
        if (self->field35e != *self->cur) {
            CalcField389_0215951c((Obj0215951c*)self);
            func_ov006_0215f4dc(self);
        }
        if (func_ov006_02158a28(self)) {
            DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
            ResetWithSub0208203c((Obj0208203c*)self->sub);
            self->cur = 0;
            if (func_ov006_02159218(self)) {
                SetEntryLowNibbleAndElement02080c68(e, self->field36e, 1);
                SetElementFlag0x40(e, self->field36e, 1);
                SetEntryLowNibbleAndElement02080c68(e, 0x11, 0);
                self->state++;
            }
        } else {
            if (func_ov006_02158b78(self)) {
                ResetWithSub0208203c((Obj0208203c*)self->sub);
                self->cur = 0;
                self->mode = 6;
                self->state = 0;
                ExpireStatusSlot02159094((Obj02159094*)self);
                func_ov006_021553bc(self->ctx, self->ids, self->mergeBuf);
                func_ov006_0215f4dc(self);
            }
            ClearField44IfSet_02158c28((StructC28*)self);
        }
    } else if (self->state == 2) {
        func_ov006_0215943c(self);
        func_ov006_021553bc(self->ctx, self->ids, self->mergeBuf);
        self->state++;
    } else if (self->state == 3) {
        if (Check02159564(self)) {
            return;
        }
        if (func_ov006_02158a28(self) && self->field35e == *self->cur) {
            unsigned char b = 0;
            unsigned char a = 0;
            short c = 0;
            func_ov006_02159274(self, &b, &c, &a);
            if (a) {
                DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
                ResetWithSub0208203c((Obj0208203c*)self->sub);
                self->cur = 0;
                SetElementFlag0x40(self->elements, 0x11, 1);
                self->field38a = b;
                self->field370 = c;
                self->field38b = 1;
                self->state++;
                self->flags &= ~0x10;
                self->flags |= 0x20;
                self->message = 9;
                self->messageTimer = 0;
                return;
            }
        } else if (func_ov006_02158b78(self)) {
            ResetWithSub0208203c((Obj0208203c*)self->sub);
            self->cur = 0;
            self->selected = -1;
            self->state = 0;
        }
        ClearField44IfSet_02158c28((StructC28*)self);
    } else if (self->state == 4) {
        UpdateEntryFlags021596f0((Struct021596f0*)self);
        self->field393 = 0;
        self->state++;
    } else if (self->state == 5) {
        self->field42c = 0;
        int r = func_ov006_02159834(self);
        if (r == 1) {
            return;
        }
        if (TestFlag0SetAndFlag1Clear(&data_02114e30, 1) || r == 2) {
            DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
            ResetWithSub0208203c((Obj0208203c*)self->sub);
            self->cur = 0;
            self->flags &= ~8;
            CallFunc0204c804OnMatchingKey(e, 6);
            func_ov006_02159108(self, self->field38a, self->field370, self->field38b);
            func_ov006_021553bc(self->ctx, self->ids, self->mergeBuf);
            self->state++;
            self->flags |= 0x20;
            self->message = 0xa;
            self->messageTimer = 0;
        } else if (func_ov006_02158b78(self) || r == 3) {
            ResetWithSub0208203c((Obj0208203c*)self->sub);
            self->cur = 0;
            self->flags &= ~8;
            CallFunc0204c804OnMatchingKey(e, 6);
            self->state = 0;
            self->message = 0x24;
            self->messageTimer = 0;
            self->flags |= 0x120;
        }
    } else if (self->state == 6) {
        func_ov006_021570fc(self->ctx, 1);
        func_ov006_02159db0(self);
        self->state++;
    } else if (self->state == 7) {
        int r = func_ov006_02159e50(self);
        if (r != -1) {
            if (r == 1) {
                self->mode = 8;
                self->busy = 0;
                self->state = 0;
            }
        } else {
            while (self->ids[0] > 0) {
                ExpireStatusSlot02159094((Obj02159094*)self);
            }
            func_ov006_021553bc(self->ctx, self->ids, self->mergeBuf);
            self->mode = 5;
            self->state = 0;
            self->flags |= 0x20;
            self->message = 0x24;
            self->messageTimer = 0;
            self->flags |= 0x100;
        }
    }
}
