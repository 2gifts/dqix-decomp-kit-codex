#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"
#include "System/Graphics.h"
#include "System/VRAM.h"

struct Struct0218b810;
struct Obj02192600;
struct Struct020A2CF0;
struct Foo0207df50;

struct CombatController021934f8 {
    char data[0x2c8];
};

struct Controller021934f8 {
    char pad0[0x195a];
    unsigned char mode;
};

struct Battle021934f8 {
    char pad0[0x3c];
    int config[4];
    int list[6];
    char lookupA[0x70 - 0x64];
    char lookupB[0x84 - 0x70];
    void* buffer;
    SafeAllocator partsAllocator;
    SafeAllocator monstersAllocator;
    SafeAllocator soundsAllocator;
    char pad_c4[0x124 - 0xc4];
    char tables[0x194 - 0x124];
    int phase;
    int unk198;
    char pad_19c[0x1a4 - 0x19c];
    int quitRequested;
    char pad_1a8[0x22c - 0x1a8];
    unsigned long long frameAccum;
    unsigned long long prevTime;
    unsigned long long curTime;
    unsigned long long avgDelta;
    float fps;
    char pad_250[0x268 - 0x250];
    char scratch[0x52];
};

extern "C" void SetWord0x0(int*, int);
extern "C" void* GetGlobal02109400();
extern "C" void BlankFunction02094b3c(void*, int);
extern "C" void BlankFunction02094b34(void*, int, int, int, int);
extern "C" void BlankFunction02094b38(void*, int);
extern "C" int AlwaysTrue02094b4c(void*);
void EncodeSignFlaggedHalfword(short*, int);
extern "C" void* func_0203bd08();
void InitBattleContext0203bd24(char*);
void LockStagedTextureVRAMCopying();
extern "C" void func_020ca458(unsigned int, unsigned int, unsigned int);
void SetDispcntModeAndFlags020c391c(int, int, int);
void SetSubBgMode(unsigned int);
void UpdateVRAMStagingVRAMBanks();
void UnlockStagedTextureVRAMCopying();
void Finish3DRendering();
extern "C" void func_020c51dc();
void ResetMatrixStacks020c537c();
void* AllocateAligned4(AllocatorUnion*, unsigned int);
void SetBuffer02028ddc(void*);
void SetStructFieldsFromCode02028d68(int);
void UploadPendingPalette02028dec(int);
void ConfigurePairMode020bb48c(unsigned int, int);
void InitGlobalStateAndInstallHandlers020bb780(void*, int);
void Init02192600(struct Obj02192600*);
void Set3DClearColor(int, int, int, int, int);
void ZeroFourFields0218b810(struct Struct0218b810*);
extern "C" void func_ov015_0218ba44(void*, int);
extern "C" void* memset(void*, int, unsigned int);
void ZeroInit020de848(void*);
extern "C" void* ExtractFileFromGP2(const char*, const char*, unsigned int*);
extern "C" void func_020dea64(void*, SafeAllocator*, void*, unsigned int, const void*, int);
void Clear12Bytes0206efc4(void*);
void LockedLookupDispatch0206efd8(void*, void*);
void ClearStruct020709d8(void*);
void LockedLookupDispatch020709ec(void*, void*);
extern "C" void func_0202df68(void*);
void ResetState020a2cf0(struct Struct020A2CF0*);
void InitCombatController020a2010(void*);
void SetField0x238False(void*);
void SetField0x3b0Value(GameState*, void*);
extern "C" void func_ov015_02192544(void*);
void SetupContextForMode0205ea20(void*, int);
void DelayThenSyncBit0();
void RestoreDisplayMode();
void InitFields02010124(GameState*);
void SetActiveDmaChannel020c3a0c(int);
unsigned long long GetCurrentTimestamp();
void* GetGlobalField0x1c020421a0();
void SetField0x1e28(void*, void*);
void CopyInternalFields0207df50(struct Foo0207df50*);
void RestorePairTables0207df90(char*);
extern "C" void func_020432c4(void*);
void BackupPairTables0207dfac(char*);
extern "C" void func_0204500c(void*, const void*, int, int);
void Forward0204359c(void*, int);
extern "C" void func_020439b0(void*, int);
extern "C" void func_ov015_02192700(void*);
void EmptyStub02012de4(void*);
void InitCombatSubsystems02012efc();
int TestFlagMask(unsigned short*, int);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);
void DispatchToActiveEntries020bc028(int);
extern "C" void func_ov015_02190e88(void*);
extern "C" void func_ov015_02191294(void*);
extern "C" void func_ov015_02191668(void*);
extern "C" void func_ov015_02190d7c(void*);
extern "C" void func_ov015_021901c8(void*);
void CleanInvalidateOamBuffers0203bd88(void*);
void ResetXYField0203aa08(void*);
extern "C" void func_020bbd9c();
extern "C" unsigned long long _ll_udiv(unsigned long long, unsigned long long);
extern "C" int _Z22GetSignedField020c39c8PVt(volatile unsigned short*);
void SyncMainSubOam0203bdb0(void*);
void FlushOrClearBuffer020290ac(int);
extern "C" void func_02094ab0(void*);

extern AllocatorUnion data_02114e20;
extern const char* data_020f2a38;
extern const char* data_020f2a30;
extern int data_ov015_02193cdc[];
extern char data_02108760;
extern int data_ov015_02194528[];
extern unsigned short data_02114e30;

#define POWCNT1 (*(volatile unsigned short*)0x04000304)
#define MASTER_BRIGHT ((short*)0x0400006c)
#define MASTER_BRIGHT_SUB ((short*)0x0400106c)
#define BG0HOFS_SUB (*(volatile int*)0x04001010)

// USA: func_ov015_021934f8
extern "C" ARM void func_ov015_021934f8(Battle021934f8* self)
{
    GameState* gs = GameState::GetInstance();
    if (gs == NULL)
        return;
    SetWord0x0((int*)gs, (int)self);
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    loader->MaybeReset();
    void* global = GetGlobal02109400();
    BlankFunction02094b3c(global, 10);
    BlankFunction02094b3c(global, 12);
    BlankFunction02094b34(global, 0x69, 0, 0, 0);
    BlankFunction02094b38(global, 0x69);
    BlankFunction02094b38(global, 0);
    while (!AlwaysTrue02094b4c(global))
        loader->RemoveAllLocks();

    EncodeSignFlaggedHalfword(MASTER_BRIGHT, -16);
    EncodeSignFlaggedHalfword(MASTER_BRIGHT_SUB, -16);
    char* context = (char*)func_0203bd08();
    InitBattleContext0203bd24(context);
    LockStagedTextureVRAMCopying();
    MapVRAMBanksToLCDC(0x1ff);
    func_020ca458(0, 0x6800000, 0xa4000);
    DisableLCDCMappedVRAMBanks();
    ReleaseTextureImageVRAMBanks();
    MapVRAMBanksToTextureImage(0xb);
    ReleaseTexturePaletteVRAMBanks();
    MapVRAMBanksToTexturePalette(0x60);
    DISPCNT = DISPCNT & ~DISPCNT_MASK_CHARACTER_BASE_64K;
    DISPCNT = DISPCNT & ~DISPCNT_MASK_SCREEN_BASE_64K;
    DISPCNT = (DISPCNT & ~0x1f00) | 0x100;
    SetDispcntModeAndFlags020c391c(1, 0, 1);
    SetSubBgMode(0);
    DISPCNTSUB = (DISPCNTSUB & ~0x1f00) | 0x1100;
    ReleaseSubBGVRAMBanks();
    MapVRAMBanksToSubBG(4);
    ReleaseSubObjVRAMBanks();
    MapVRAMBanksToSubObj(0x100);
    DISPCNTSUB = (DISPCNTSUB & 0xffcfffef) | 0x10;
    UpdateVRAMStagingVRAMBanks();
    UnlockStagedTextureVRAMCopying();
    EncodeSignFlaggedHalfword(MASTER_BRIGHT, 0);
    EncodeSignFlaggedHalfword(MASTER_BRIGHT_SUB, 0);
    POWCNT1 = POWCNT1 & ~0x8000;
    Finish3DRendering();
    func_020c51dc();
    ResetMatrixStacks020c537c();
    BG1CNT = BG1CNT & ~3;
    BG0CNT = (BG0CNT & ~3) | 1;
    BG0CNTSUB = BG0CNTSUB & ~3;
    BG0HOFS_SUB = 0;
    self->buffer = AllocateAligned4(&data_02114e20, 0x6000);
    SetBuffer02028ddc(self->buffer);
    SetStructFieldsFromCode02028d68(4);
    UploadPendingPalette02028dec(1);
    ConfigurePairMode020bb48c(3, 1);
    InitGlobalStateAndInstallHandlers020bb780((void*)0x8000, 1);
    Init02192600((struct Obj02192600*)self);
    Set3DClearColor(0x594a, 0x10, 0x7fff, 0, 0);
    ZeroFourFields0218b810((struct Struct0218b810*)&self->config);
    func_ov015_0218ba44(&self->config, 0);
    memset(self->scratch, 0, 0x52);

    self->partsAllocator.ResetAllocatorPointer();
    self->partsAllocator.CreateTypeA(AllocateAligned4(&data_02114e20, 0x17600), 0x17600);
    self->partsAllocator.Reset();
    ZeroInit020de848(self->list);
    BackgroundLoader::AddLockGlobal();
    unsigned int size;
    void* data;
    size = 0;
    data = ExtractFileFromGP2(data_020f2a38, data_020f2a30, &size);
    if (data != NULL)
        func_020dea64(self->list, &self->partsAllocator, data, size, data_ov015_02193cdc, 9);
    BackgroundLoader::RemoveLockGlobal();

    self->monstersAllocator.ResetAllocatorPointer();
    self->monstersAllocator.CreateTypeA(AllocateAligned4(&data_02114e20, 0x20000), 0x20000);
    self->monstersAllocator.Reset();
    Clear12Bytes0206efc4(self->lookupA);
    LockedLookupDispatch0206efd8(self->lookupA, &self->monstersAllocator);

    self->soundsAllocator.ResetAllocatorPointer();
    self->soundsAllocator.CreateTypeA(AllocateAligned4(&data_02114e20, 0x20000), 0x20000);
    self->soundsAllocator.Reset();
    ClearStruct020709d8(self->lookupB);
    LockedLookupDispatch020709ec(self->lookupB, &self->soundsAllocator);

    CombatController021934f8 combat;
    func_0202df68(&combat);
    ResetState020a2cf0((struct Struct020A2CF0*)&combat);
    InitCombatController020a2010(&combat);
    SetField0x238False(&combat);
    SetField0x3b0Value(gs, &combat);
    func_ov015_02192544(self);
    SetupContextForMode0205ea20(&data_02108760, 0x65);
    DelayThenSyncBit0();
    RestoreDisplayMode();
    DISPCNTSUB = DISPCNTSUB | 0x10000;
    InitFields02010124(gs);
    SetActiveDmaChannel020c3a0c(0);
    GetCurrentTimestamp();
    Controller021934f8* controller = (Controller021934f8*)GetGlobalField0x1c020421a0();
    SetField0x1e28(controller, self->tables);
    CopyInternalFields0207df50((struct Foo0207df50*)self->tables);
    RestorePairTables0207df90(self->tables);
    func_020432c4(controller);
    BackupPairTables0207dfac(self->tables);
    func_0204500c(controller, data_ov015_02194528, 0, 0xe3);
    controller->mode = 8;
    for (unsigned int i = 0; i < 0x10; i++)
    {
        Forward0204359c(controller, 0x10);
        func_020439b0(controller, 0);
    }
    self->phase = 0;
    self->unk198 = 2;
    self->quitRequested = 0;
    func_ov015_02192700(self);
    EmptyStub02012de4(&data_02114e20);
    unsigned long long last = GetCurrentTimestamp();

    for (;;)
    {
        InitCombatSubsystems02012efc();
        int quit;
        if (TestFlagMask(&data_02114e30, 4) && TestFlagMask(&data_02114e30, 8))
            quit = 1;
        else if (self->quitRequested == 0 && TestFlag0SetAndFlag1Clear(&data_02114e30, 2))
            quit = 1;
        else
            quit = 0;
        if (quit)
        {
            DispatchToActiveEntries020bc028(0);
            break;
        }
        if (TestFlag0SetAndFlag1Clear(&data_02114e30, 8))
            func_ov015_02192544(self);
        switch (self->phase)
        {
        case 0:
            func_ov015_02190e88(self);
            break;
        case 1:
            func_ov015_02191294(self);
            break;
        case 2:
            func_ov015_02191668(self);
            break;
        }
        func_ov015_02190d7c(self);
        func_ov015_021901c8(self);
        CleanInvalidateOamBuffers0203bd88(context);
        ResetXYField0203aa08(&data_02108760);
        func_020bbd9c();

        unsigned long long oldTime = self->curTime;
        self->curTime = _ll_udiv(GetCurrentTimestamp() * 64000, 33514);
        if (self->prevTime < self->curTime)
        {
            unsigned long long delta = self->curTime - self->prevTime;
            self->avgDelta = (delta + self->avgDelta) >> 1;
        }
        if (oldTime < self->curTime)
        {
            self->frameAccum += _ll_udiv(self->curTime - oldTime, 1000);
            if (self->frameAccum > 1000)
            {
                self->frameAccum = 0;
                self->fps = 1000000.0f / (float)self->avgDelta;
            }
        }
        loader->RemoveAllLocks();
        self->prevTime = _ll_udiv(GetCurrentTimestamp() * 64000, 33514);
        if (_Z22GetSignedField020c39c8PVt((volatile unsigned short*)MASTER_BRIGHT) > -16)
            DelayThenSyncBit0();
        GetCurrentTimestamp();
        SyncMainSubOam0203bdb0(context);
        FlushOrClearBuffer020290ac(1);
        gs->CalculateDeltaTime(_ll_udiv((GetCurrentTimestamp() - last) * 64000, 33514));
        last = GetCurrentTimestamp();
    }

    BlankFunction02094b38(global, 0x69);
    func_02094ab0(global);
    SetupContextForMode0205ea20(&data_02108760, 0x64);
    InitCombatController020a2010(&combat);
    ResetState020a2cf0((struct Struct020A2CF0*)&combat);
    func_0202df68(&combat);
}
