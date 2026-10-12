#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Memory/SafeAllocator.h"
#include "System/Graphics.h"
#include "System/Interrupts.h"
#include "System/VRAM.h"

#define MASTER_BRIGHT (*(volatile unsigned short*)0x0400006c)
#define MASTER_BRIGHT_SUB (*(volatile unsigned short*)0x0400106c)
#define REG_IME (*(volatile unsigned short*)0x04000208)
#define BG0OFSSUB (*(volatile unsigned int*)0x04001010)

struct GameState {
    static GameState* GetInstance();
};

struct Info_0218caa8 {
    char pad0[0x10];
    unsigned char isSet;
    char pad11;
    short id;
};

struct State_0218caa8 {
    int mute;
    int flag4;
    SafeAllocator* alloc;
    char padc[4];
    int started;
    char pad14[0x34 - 0x14];
    Info_0218caa8* info;
    char pad38[0x40 - 0x38];
    unsigned long long startTime;
};

struct BackgroundGraphics0218caa8 {
    char unk_0[0x1c];
    unsigned char screen : 4;
    unsigned char palette : 4;
    unsigned char unk_1d;
    char unk_1e[2];
};

struct AllocatorUnion;
struct ActiveEntry02046900;
struct Rec020467f0;
struct List0204af64;
struct AllocTarget0204b12c;
struct Foo0204af38;
struct Obj0204b8d0;
struct List0204b0e8;
struct Struct0200fb08;

void EncodeSignFlaggedHalfword(short* reg, int brightness);
void ClearField0x0_020bc078(int* field);
void CallField0xa00203aa44(void* obj);
int TryAllocateOrAppend0203aaf8(void* obj, void* arg1, int* out);
int SetVBlankIrqEnable(int enable);
int GetWord0x7f6c(void* gs);
extern "C" void func_020ca458(unsigned int value, void* dest, int count);
void SetSubBgMode(unsigned int mode);
void* AllocateAligned4(AllocatorUnion* alloc, unsigned int size);
void TailForward02012da4(AllocatorUnion* alloc, void* data);
void ResetList0204af64(List0204af64* graphics);
extern "C" void func_0204b5b4(void* graphics, int priority);
void SetWord0x18ClearByte0x1f(unsigned char* graphics, int value);
void AllocateAndClearBuffer0204b12c(AllocTarget0204b12c* graphics, SafeAllocator* allocator);
void AllocateArray0204af38(Foo0204af38* graphics, int count, SafeAllocator* allocator);
int CountActiveEntries(ActiveEntry02046900* pac);
void* FindRecordByIndex(Rec020467f0* pac, int index, void** outName, int* outSize);
extern "C" void func_0204b174(void* graphics, void* file, SafeAllocator* allocator, unsigned int size);
unsigned char NormalizeField5_0200fb08(Struct0200fb08* obj);
void DispatchEntry0204b8d0(Obj0204b8d0* graphics, unsigned int a, int b, int c, short d, short e, short f, short g, unsigned short h);
void FlushAndDispatchList0204b0e8(List0204b0e8* graphics, void* characters);
void DelayThenSyncBit0(void);
extern "C" void func_ov016_0218c04c(const char* path, int arg1);
void EnsureInitAndSetFlag_0218be1c(void);
void InitOverlay17ObjAndBumpBattleCounter02012bd8(void);

extern State_0218caa8 data_ov016_0219d0c0;
extern int data_ov016_0219d0cc;
extern char data_02109bf4;
extern AllocatorUnion data_02114e20;
extern char data_ov016_0219d030[];

// USA: func_ov016_0218caa8
extern "C" ARM void func_ov016_0218caa8(const char* path, SafeAllocator* alloc, Info_0218caa8* info, int mute, int arg4) {
    EncodeSignFlaggedHalfword((short*)&MASTER_BRIGHT, -16);
    EncodeSignFlaggedHalfword((short*)&MASTER_BRIGHT_SUB, -16);
    data_ov016_0219d0c0.info = info;
    data_ov016_0219d0c0.startTime = 0;
    data_ov016_0219d0c0.started = info->id < 0;
    data_ov016_0219d0c0.mute = mute;
    ClearField0x0_020bc078(&data_ov016_0219d0cc);
    if (data_ov016_0219d0c0.info->isSet == 0) {
        CallField0xa00203aa44(&data_02109bf4);
        TryAllocateOrAppend0203aaf8(&data_02109bf4, (void*)(int)data_ov016_0219d0c0.info->id, 0);
    }
    SetInterruptHandler(1, (const void*)EnsureInitAndSetFlag_0218be1c);
    EnableSpecificInterrupts(0x60078);
    REG_IME;
    REG_IME = 1;
    SetVBlankIrqEnable(1);
    GameState* gs = GameState::GetInstance();
    if (GetWord0x7f6c(gs) == 5) {
        MapVRAMBanksToLCDC(0x1ff);
        func_020ca458(0, (void*)0x06800000, 0xa4000);
        func_020ca458(0xc0, (void*)0x07000000, 0x400);
        func_020ca458(0, (void*)0x05000000, 0x400);
        func_020ca458(0xc0, (void*)0x07000400, 0x400);
        func_020ca458(0, (void*)0x05000400, 0x400);
        MapVRAMBanksToLCDC(3);
        MapVRAMBanksToSubBG(0x80);
        SetSubBgMode(0);
        DISPCNTSUB = (DISPCNTSUB & ~0x1f00) | 0x100;
        BG0CNTSUB = BG0CNTSUB & ~3;
        BG0CNTSUB = (BG0CNTSUB & 0x43) | 0xf00;
        BG0OFSSUB = 0;
        void* buffer = AllocateAligned4(&data_02114e20, 0x2000);
        SafeAllocator allocator;
        allocator.ResetAllocatorPointer();
        allocator.ResetAllocatorPointer();
        allocator.CreateTypeA(buffer, 0x2000);
        BackgroundGraphics0218caa8 background;
        ResetList0204af64((List0204af64*)&background);
        background.screen = 1;
        background.palette = 0;
        func_0204b5b4(&background, 0);
        SetWord0x18ClearByte0x1f((unsigned char*)&background, 0);
        AllocateAndClearBuffer0204b12c((AllocTarget0204b12c*)&background, &allocator);
        AllocateArray0204af38((Foo0204af38*)&background, 5, &allocator);
        int task;
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        loader->MaybeReset();
        task = loader->QueueLoadFile(data_ov016_0219d030, NULL);
        while (true) {
            if (loader->GetTaskStatus(task)) {
                void* name;
                void* file;
                unsigned int size;
                loader->GetLoadedFileByID(task, &file, &size);
                int numFiles = CountActiveEntries((ActiveEntry02046900*)file);
                for (int i = 0; i < numFiles; i++) {
                    int dataSize;
                    void* data = FindRecordByIndex((Rec020467f0*)file, i, &name, &dataSize);
                    if (data != NULL) {
                        func_0204b174(&background, data, &allocator, dataSize);
                    }
                }
                loader->RemoveTask(task);
                int frame = 0;
                switch (NormalizeField5_0200fb08((Struct0200fb08*)gs)) {
                case 2:
                    frame = 1;
                    break;
                case 3:
                    frame = 3;
                    break;
                case 4:
                    frame = 2;
                    break;
                case 5:
                    frame = 4;
                    break;
                }
                DispatchEntry0204b8d0((Obj0204b8d0*)&background, 0, 0, 0, 0, 0, 0x20, 0x19, 0xffff);
                if (frame > 0) {
                    DispatchEntry0204b8d0((Obj0204b8d0*)&background, (unsigned char)frame, 0, 0, 0, 0x11, 0x20, 0x19, 0xffff);
                }
                FlushAndDispatchList0204b0e8((List0204b0e8*)&background, NULL);
                break;
            }
            loader->RemoveAllLocks();
        }
        if (allocator.GetSignedAllocator() != NULL) {
            allocator.Destroy();
        }
        if (buffer != NULL) {
            TailForward02012da4(&data_02114e20, buffer);
        }
    } else {
        MapVRAMBanksToLCDC(0x1ff);
        func_020ca458(0, (void*)0x06800000, 0xa4000);
        DisableLCDCMappedVRAMBanks();
        func_020ca458(0xc0, (void*)0x07000000, 0x400);
        func_020ca458(0, (void*)0x05000000, 0x400);
        func_020ca458(0xc0, (void*)0x07000400, 0x400);
        func_020ca458(0, (void*)0x05000400, 0x400);
        MapVRAMBanksToLCDC(0x1ff);
    }
    DelayThenSyncBit0();
    data_ov016_0219d0c0.alloc = alloc;
    EncodeSignFlaggedHalfword((short*)&MASTER_BRIGHT, 0);
    EncodeSignFlaggedHalfword((short*)&MASTER_BRIGHT_SUB, 0);
    func_ov016_0218c04c(path, arg4);
    SetInterruptHandler(1, (const void*)InitOverlay17ObjAndBumpBattleCounter02012bd8);
}
