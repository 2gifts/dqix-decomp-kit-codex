#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"
#include "Resource/GameResources.h"

struct Struct0205a198 {
    unsigned char raw[0x28];
};

struct GlobalData020e081c {
    unsigned char pad0[0x508];
    int field_508;
};

struct Obj020e081c {
    unsigned char pad0[0x94];
    unsigned char block94[0xd4 - 0x94];
    void* field_d4;
    unsigned char pad_d8[0xe0 - 0xd8];
    short field_e0;
    unsigned char pad_e2[0xe4 - 0xe2];
    unsigned char field_e4;
    unsigned char pad_e5[0xe8 - 0xe5];
    struct Struct0205a198 entries_e8[0x1c];
    unsigned char pad_548[0x76c - 0x548];
    SafeAllocator* alloc_76c;
    void* ptr_770;
    unsigned int size_774;
    unsigned char pad_778[0x9c2 - 0x778];
    signed char field_9c2;
    unsigned char field_9c3;
    unsigned char pad_9c4[0x9fc - 0x9c4];
    int handle_9fc;
    unsigned char pad_a00[0xa08 - 0xa00];
    int handle_a08;
    unsigned char pad_a0c[0xffc - 0xa0c];
    int field_ffc;
};

struct SetFlagStruct;
void SetFlag0x9c6(struct SetFlagStruct* obj, int value);
extern "C" void func_02021428(void* obj, int value);
void InitSubScreenDisplay(void* obj);
void TailForward0203b66c(char* s);
void InitBattleContext0203bd24(char* s);
void Init0205a198(struct Struct0205a198* entry);
void InitStruct0205a444(char* s);
void* GetData02105254();
void* GetGlobalPtr02105244();
extern "C" void* func_0203be4c();
extern "C" void* func_0203bd08();
extern "C" void func_02012fe4();
extern "C" void func_020c9be0();
void DelayThenSyncBit0();
void FinalizeRecordListDisplay02020cb8(void* obj, int* list);
void ReleaseHandle02022b90(void* owner, int* handle);
void SetToggleState(struct ToggleState* obj, int a, int b);
void ProcessRecords0205a498(void* obj, struct Rec020467f0* rec, int count, void* b);
int IsAxisIntWithin16(struct AxisFloats0203b5a0* a, int v);
extern "C" void VectorizedInvertedMemcpy(void* dst, void* src, unsigned int size);

extern int data_020f2b78;
extern int data_020f2b90;
extern int data_020f2ba2;

// USA: func_020e081c
extern "C" ARM void func_020e081c(struct Obj020e081c* self) {
    GameState::GetInstance();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    func_02012fe4();
    GameResources* a = func_ov017_0218b5b0();
    SafeAllocator* b = (SafeAllocator*)((char*)a + 0x128);
    void* c = func_0203bd08();
    void* sb = GetData02105254();
    signed char st = self->field_9c2;

    if (st == 0) {
        SetFlag0x9c6((struct SetFlagStruct*)self, 0);
        func_02021428(self, 0x19);
        InitSubScreenDisplay(self);
        b->Reset();
        TailForward0203b66c((char*)sb);
        InitBattleContext0203bd24((char*)c);
        for (int i = 0; i < 0x1c; i++) {
            Init0205a198(&self->entries_e8[i]);
        }
        InitStruct0205a444((char*)self->block94);
        self->field_e4 = 1;
        self->field_d4 = self->entries_e8;
        self->field_e0 = 5;
        loader->AddFence();
        self->handle_9fc = loader->QueueLoadFile((const char*)&data_020f2b78, 0);
        self->handle_a08 = loader->QueueLoadFileInGP2((const char*)&data_020f2b90, (const char*)&data_020f2ba2, 0);
        self->field_9c2 = self->field_9c2 + 1;
        return;
    }

    if (st != 1) {
        return;
    }

    if (loader->GetTaskStatus(self->handle_9fc) == 0) {
        return;
    }
    if (loader->GetTaskStatus(self->handle_a08) == 0) {
        return;
    }
    func_02021428(self, 0x19);
    if (IsAxisIntWithin16((struct AxisFloats0203b5a0*)a, 1) != 0) {
        return;
    }

    void* rec = 0;
    unsigned int len = 0;
    loader->GetLoadedFileByID(self->handle_a08, &rec, &len);
    ProcessRecords0205a498(self->block94, (struct Rec020467f0*)rec, (int)len, (void*)b);

    GlobalData020e081c* gp = (GlobalData020e081c*)GetGlobalPtr02105244();
    gp->field_508 = 0x2800;
    func_0203bd08();
    void* d = (char*)func_0203be4c() + 0x258;
    *(void**)GetGlobalPtr02105244() = d;

    if (self->alloc_76c == 0) {
        func_020c9be0();
    }
    if (self->alloc_76c != 0 && self->ptr_770 != 0) {
        self->alloc_76c->Free(self->ptr_770);
    }
    self->size_774 = 0x3400;
    self->ptr_770 = self->alloc_76c->Allocate(0x3400);
    VectorizedInvertedMemcpy((void*)0x6600000, self->ptr_770, self->size_774);

    DelayThenSyncBit0();
    FinalizeRecordListDisplay02020cb8(self, &self->field_ffc);
    ReleaseHandle02022b90(self, &self->handle_a08);
    SetToggleState((struct ToggleState*)self, 1, 3);
    self->field_9c3 = 0;
    self->field_9c2 = self->field_9c2 + 1;
}
