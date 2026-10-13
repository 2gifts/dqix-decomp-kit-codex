#include <globaldefs.h>
#include "std_library_functions.h"
#include "System/Memory.h"
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"

extern char data_0211e33c[];
extern char data_020f1bf0[];

struct StateBits5ccc_11544;
struct Buf020a94f8 {
    int hdr;
    char pad4[4];
    char name[12];
};
struct Sub5000_020ab314 {
    char pad0[0xcc8];
    unsigned char flag_cc8;
};

extern "C" int func_02075910(int a, void* p, unsigned int size, int c);
extern "C" int func_01ff85b8(void* buf, int size);
extern "C" void* func_0205ec34(void);
extern "C" void func_020a95a4(void* a, int b, char* c);
extern "C" int func_020a9a50(void* ctx, char* dst, int flag);
extern "C" void func_020a9b2c(void* p);
extern "C" void func_020a9c14(void* p);
extern "C" int func_020a9e30(void* param);

int VerifyBuffer020a94f8(struct Buf020a94f8* buf, int flag);
int VerifyAndApplySaveBuffer(int flag);
void SetFlag0x5cccBit0(struct StateBits5ccc_11544* state);
int GetSlotEntry020108d8(char* p, int idx);
void* GetGlobal02109418(void);
void LoadBattleRegion0x5cdc(unsigned char* p);
void CopyBlobIntoBattleStruct020a9ba4(void* p);
void CopyBlobIntoBattleStruct020a9d78(void* p);

// USA: func_020ab314
extern "C" ARM int TODO_Name_020ab314(int, int, int flag) {
    GameState* gs = GameState::GetInstance();
    ((struct Sub5000_020ab314*)((char*)gs + 0x5000))->flag_cc8 = 0;
    BackgroundLoader::AddLockGlobal();
    BackgroundLoader::FreeAllocationsGlobal();
    int ok = 1;
    int sel = flag ? 0x8084 : 0x84;
    unsigned char* data = (unsigned char*)&data_0211e33c[0];
    unsigned char* base = data + 0x28000;
    VectorizedMemset(base, 0, 0x8000);
    if (func_02075910(0, base, 0x10, 0) == 0) {
        ok = 0;
    }
    struct Buf020a94f8 buf;
    if (VerifyBuffer020a94f8(&buf, flag != 0) == 0) {
        ok = 0;
    }
    VectorizedInvertedMemcpy(&buf, base + 0x10, 0x14);
    VerifyAndApplySaveBuffer(flag ? 1 : 0);
    VectorizedInvertedMemcpy((char*)gs + 0x2380 + 0x4000, base + 0x24, 0x54);
    if (func_02075910(sel, base + 0x84, 0x6f60, 0) == 0) {
        ok = 0;
    }
    unsigned char* blk = base + 0x84;
    if (strcmp(buf.name, data_020f1bf0) == 0 && ok) {
        if (*(int*)blk != func_01ff85b8(blk + 4, 0x6f5c)) {
            *((unsigned char*)gs + 0x5cc8) = flag ? 3 : 2;
            ok = 0;
        }
    }
    if (ok == 0) {
        SetFlag0x5cccBit0((struct StateBits5ccc_11544*)gs);
        BackgroundLoader::RemoveLockGlobal();
        return 0;
    }
    void* ctx = func_0205ec34();
    void* glob = GetGlobal02109418();
    unsigned char tmp = 0;
    int slot = GetSlotEntry020108d8((char*)gs, 0);
    func_020a95a4(blk + 4, slot, (char*)&tmp);
    func_020a9a50(blk + 0xac + 0x2c00, (char*)ctx, 0);
    memcpy(glob, blk + 0xff0 + 0x2000, 0xa0);
    memcpy((char*)GetGlobal02109418() + 0x178, blk + 0x90 + 0x3000, 0x330);
    LoadBattleRegion0x5cdc(blk + 0x33c0);
    memcpy((char*)gs + 0x5f0 + 0x7000, blk + 0x154 + 0x3400, 0x4d0);
    memcpy((char*)gs + 0x3ac0 + 0x4000, blk + 0x224 + 0x3800, 0x3b4);
    memcpy((char*)gs + 0x104 + 0x7400, blk + 0x1d8 + 0x3c00, 0xec);
    func_020a9b2c(blk + 0x2c4 + 0x3c00);
    CopyBlobIntoBattleStruct020a9ba4(blk + 0x204 + 0x5800);
    func_020a9c14(blk + 0x208 + 0x5c00);
    GameState* g2 = GameState::GetInstance();
    memcpy((char*)g2 + 0x29c + 0x5400, blk + 0x8e0 + 0x6000, 0x7c);
    if (flag != 0) {
        CopyBlobIntoBattleStruct020a9d78(blk + 0x15c + 0x6800);
        func_020a9e30(blk + 0x14c + 0x6c00);
    }
    GameState* g3 = GameState::GetInstance();
    memcpy((char*)g3 + 0x1fc + 0x7000, blk + 0x178 + 0x6800, 0x2c4);
    GameState* g4 = GameState::GetInstance();
    memcpy((char*)g4 + 0x26c + 0x5c00, blk + 0x3c + 0x6c00, 0x110);
    BackgroundLoader::RemoveLockGlobal();
    return 1;
}
