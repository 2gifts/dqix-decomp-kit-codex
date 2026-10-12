#include <globaldefs.h>
#include "System/Graphics.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"

struct ZeroTarget020de848 {
    char data[0x18];
};
struct List0204af64 {
    char data[0x20];
};
struct InitTarget0205cfd4 {
    char data[0xbc];
};
struct Struct_0205bef8 {
    char data[0x40];
};
struct Obj02092aa4 {
    char data[0x3c];
};
struct State0xc0cc {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    short unk14;
};
struct Canvas0204c684 {
    char data[0xe0];
};
struct Combatant_021723a8 {
    ZeroTarget020de848* link0;
    char* link4;
    char pad8[0x30];
    InitTarget0205cfd4* link38;
    char pad3c[8];
    int unk44;
    int unk48;
    char pad4c[0x448 - 0x4c];
};

void ZeroInit020de848(void*);
void ResetList0204af64(struct List0204af64*);
void InitStruct0205cfd4(struct InitTarget0205cfd4*);
void Init0205bef8(struct Struct_0205bef8*);
extern "C" void func_0204c684(void*);
void InitObj02092aa4(struct Obj02092aa4*, unsigned char);
void InitStruct0205a444(char*);
extern "C" void* memset(void*, int, unsigned int);
extern "C" void* func_0202ae18();
int GetField0x3acValue(GameState*);
extern "C" void func_ov000_0217228c(void*);
void InitFields0x0(struct State0xc0cc*);
extern "C" void func_ov000_0216ff28(void*);
extern "C" void func_ov000_0217feb0(void*, int, int);
extern "C" void func_ov000_02174b14(void*);

struct Owner_021723a8 {
    char pad0[0x20];
    int field20;
    char pad24[0x68 - 0x24];
    int field68;
    signed char order[4];
    char pad70[0x7c - 0x70];
    Obj02092aa4 obj7c;
    char pad_b8[0xb8 - 0x7c - sizeof(Obj02092aa4)];
    char objb8[0x18];
    ZeroTarget020de848 zeroD0;
    ZeroTarget020de848 zeroE8;
    char pad100[0x11c - 0x100];
    char obj11c[0x54];
    int field170;
    int field174;
    int field178;
    int field17c;
    unsigned short bg0;
    unsigned short bg1;
    int field184;
    InitTarget0205cfd4 init188;
    Struct_0205bef8 init244;
    Canvas0204c684 canvases[7];
    List0204af64 list8a4;
    List0204af64 list8c4;
    List0204af64 list8e4;
    char pad904[4];
    int field908;
    int field90c;
    int field910;
    int field914;
    int field918;
    int field91c;
    int field920;
    int field924;
    int field928;
    int field92c;
    char pad930[4];
    int field934;
    char pad938[4];
    int field93c;
    int field940;
    int field944;
    char pad948[8];
    int field950;
    unsigned char field954;
    unsigned char field955;
    unsigned char field956;
    char pad957[1];
    Combatant_021723a8 slots[4];
    SafeAllocator alloc1a78;
    SafeAllocator alloc1a8c;
    SafeAllocator alloc1aa0;
    SafeAllocator alloc1ab4;
    SafeAllocator alloc1ac8;
    SafeAllocator alloc1adc;
    SafeAllocator* parentAlloc;
    char pad1af4[4];
    SafeAllocator alloc1af8;
    State0xc0cc states1b0c[3];
    State0xc0cc state1b54;
    State0xc0cc state1b6c;
    State0xc0cc states1b84[9];
    State0xc0cc states1c5c[8];
    char pad1d1c[0x1d5c - 0x1d1c];
    unsigned char field1d5c;
    char pad1d5d[1];
    short field1d5e;
    unsigned char flagsArr[8];
    signed char flagIdx;
    unsigned char field1d69;
    char pad1d6a[1];
    signed char field1d6b;
    signed char field1d6c;
    char pad1d6d[1];
    signed char field1d6e;
    signed char field1d6f;
    unsigned char field1d70;
    char pad1d71[1];
    unsigned short flags;
    char pad1d74[4];
    unsigned char field1d78;
    unsigned char field1d79;
    unsigned char field1d7a;
    char pad1d7b[1];
    int field1d7c;
    unsigned char field1d80;
    unsigned char field1d81;
};

struct Data02184288 {
    int field0;
    int field4;
    Owner_021723a8* owner;
};
extern struct Data02184288 data_ov000_02184288;

// USA: func_ov000_021723a8
extern "C" ARM void func_ov000_021723a8(Owner_021723a8* self, SafeAllocator* parent) {
    self->field20 = -1;
    self->alloc1a78.ResetAllocatorPointer();
    self->alloc1a8c.ResetAllocatorPointer();
    self->alloc1ab4.ResetAllocatorPointer();
    self->alloc1aa0.ResetAllocatorPointer();
    self->alloc1ac8.ResetAllocatorPointer();
    self->alloc1adc.ResetAllocatorPointer();
    self->alloc1af8.ResetAllocatorPointer();
    ZeroInit020de848(&self->zeroD0);
    ZeroInit020de848(&self->zeroE8);
    data_ov000_02184288.owner = self;
    self->field68 = 0;
    self->field178 = 0;
    self->field17c = -1;
    self->flags = 0;
    self->flags |= 2;
    self->field184 = 0;
    volatile short v0, v1;
    unsigned short raw0 = BG0CNTSUB;
    v0 = raw0;
    self->bg0 = raw0;
    v0;
    unsigned short raw1 = BG1CNTSUB;
    v1 = raw1;
    self->bg1 = raw1;
    v1;
    ResetList0204af64(&self->list8a4);
    ResetList0204af64(&self->list8c4);
    ResetList0204af64(&self->list8e4);
    InitStruct0205cfd4(&self->init188);
    Init0205bef8(&self->init244);
    for (int i = 0; i < 7; i++) {
        func_0204c684(&self->canvases[i]);
    }
    self->parentAlloc = parent;
    InitObj02092aa4(&self->obj7c, 0);
    InitStruct0205a444(self->obj11c);
    self->field170 = 0;
    self->field174 = 0;
    self->field910 = 0;
    self->field934 = 0;
    self->field908 = 0;
    self->field90c = -1;
    self->field914 = 0;
    self->field918 = 0;
    self->field91c = 0;
    self->field920 = 0x100;
    self->field924 = 0x100;
    self->field928 = 0x100;
    self->field92c = 0;
    self->field1d5e = 0x10;
    self->field93c = 0;
    self->field940 = 0;
    self->field944 = 0;
    memset(self->flagsArr, 0, 8);
    self->flagIdx = 0;
    self->flagsArr[self->flagIdx] = 1;
    self->field1d69 = 0;
    self->field1d6b = -1;
    self->field1d6c = -1;
    self->field1d6e = -1;
    self->field1d6f = -1;
    self->field1d70 = 0;
    self->field1d79 = 1;
    self->field1d80 = 0;
    self->field1d81 = 0;
    self->field1d5c = 0;
    self->field1d7a = 0;
    GameState* state = GameState::GetInstance();
    func_0202ae18();
    int value = GetField0x3acValue(state);
    if (value < 0) {
        value = 0;
    }
    self->field17c = value;
    func_ov000_0217228c(self);
    InitFields0x0(&self->state1b54);
    InitFields0x0(&self->state1b6c);
    for (int i = 0; i < 9; i++) {
        InitFields0x0(&self->states1b84[i]);
    }
    for (int i = 0; i < 3; i++) {
        InitFields0x0(&self->states1b0c[i]);
    }
    for (int i = 0; i < 8; i++) {
        InitFields0x0(&self->states1c5c[i]);
    }
    Combatant_021723a8* slot;
    for (int i = 0; i < 4; i++) {
        slot = &self->slots[self->order[i]];
        func_ov000_0216ff28(slot);
        slot->unk44 = 0;
        slot->unk48 = 0;
        slot->link38 = &self->init188;
        slot->link4 = self->objb8;
        slot->link0 = &self->zeroD0;
    }
    func_ov000_0217feb0(self, 0, -1);
    func_ov000_0217feb0(self, 1, GetField0x3acValue(state));
    func_ov000_02174b14(self);
    self->field1d7c = 0;
    self->field1d78 = 0;
    self->field950 = 0;
    self->field954 = 0;
    self->field955 = 0;
    self->field956 = 0;
}
