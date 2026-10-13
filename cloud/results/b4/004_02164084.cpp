#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/FileIO.h"
#include "Grotto/Main/TreasureMapDataStructs.h"

struct MapDisplayBuffer {
    TreasureMapMetadata maps[99];
    DetailedTreasureMapData details[8];
    unsigned char count;
    char pad18f5;
    short page;
    short field5e;
};
struct MapDisplayPointers { MapDisplayBuffer* buffer; void* scratch; void* file; };
struct MapAllocatorNode { int field0; SafeAllocator allocator; };

struct Grid021f9b30 {
    char pad[0x5c];
    short page;
    short field_0x5e;
    char pad60;
    unsigned char ready;
};

class Widget02164084 {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual void v47();
    virtual void v48();
    virtual void v49();
    virtual void v50();
    virtual void v51();
    virtual void v52();
    virtual void v53();
    virtual void Notify(int code);

    char pad4[8];
    unsigned char flags;
    char padd[0x13];
    char* text;
    char pad24[0xe0];
    short page;
    short field5e;
};

extern MapDisplayPointers data_ov004_02171010;
extern char data_ov004_02170584[];
extern "C" void* func_ov011_021849c8(void*);
extern "C" Widget02164084* func_ov023_021f6880(void*, int);
extern "C" int func_ov023_021f6f10(void*);
extern "C" void func_ov023_021f65d4(void*, int, int);
extern "C" void func_ov023_021f9d14(void*, void*);
extern "C" void func_ov023_021f809c(void*, void*);
extern "C" MapAllocatorNode* func_ov011_021845f8(void*, int);
void SetField3aIfFound_021634b8(void*, int, int);
extern "C" void _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(Grid021f9b30*, unsigned short, unsigned int, unsigned int);
void ClearNodeMaskById_021f6600(void*, int, int);
int CopyToRegion0x6482IfDst(char*, void*);
struct Info02011930;
int GetAndConsumeNameEntries02011930(void*, Info02011930*, void*, void*);

// USA: func_ov004_02164084
extern "C" ARM int func_ov004_02164084(void* obj) {
    void* ov011 = func_ov011_021849c8(obj);
    Grid021f9b30* grid = (Grid021f9b30*)func_ov023_021f6880(ov011, 10);
    if (!grid) return 0;
    if (func_ov023_021f6f10(grid) != 7) return 0;
    short page0 = data_ov004_02171010.buffer->page;
    short field0 = data_ov004_02171010.buffer->field5e;
    grid->page = page0;
    grid->field_0x5e = field0;
    BackgroundLoader::AddLockGlobal();
    MapAllocatorNode* node = func_ov011_021845f8(obj, 0);
    if (!node) return 0;
    SafeAllocator* allocator = &node->allocator;
    if (!allocator) return 0;
    unsigned char* scratch = (unsigned char*)allocator->Allocate(0x8000);
    unsigned int length = 0;
    if (!LoadFileIntoMemory(data_ov004_02170584, scratch, &length)) {
        BackgroundLoader::RemoveLockGlobal();
        return 0;
    }
    for (int i = 0; i < 8; i++) {
        int index = i + data_ov004_02171010.buffer->page * 8;
        Widget02164084* left = func_ov023_021f6880(ov011, i + 0x3c);
        if (!left) continue;
        if (func_ov023_021f6f10(left) != 8) return 0;
        Widget02164084* right = func_ov023_021f6880(ov011, i + 0x46);
        if (!right) continue;
        if (func_ov023_021f6f10(right) != 8) return 0;
        func_ov023_021f65d4(obj, i + 0x226, 8);
        SetField3aIfFound_021634b8(obj, i + 0x226, 0);
        func_ov023_021f65d4(obj, i + 0x230, 8);
        SetField3aIfFound_021634b8(obj, i + 0x230, 0);
        func_ov023_021f65d4(obj, i + 0x23a, 8);
        SetField3aIfFound_021634b8(obj, i + 0x23a, 0);
        DetailedTreasureMapData* det = &data_ov004_02171010.buffer->details[i];
        det->BlankFunction();
        left->text = 0;
        left->flags |= 8;
        _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(grid, 0, i & 0xffff, 0);
        right->text = 0;
        right->flags |= 8;
        if (index >= data_ov004_02171010.buffer->count) continue;
        ExportDetailedTreasureMapData(&data_ov004_02171010.buffer->maps[index], &data_ov004_02171010.buffer->details[i], true, scratch);
        left->flags &= ~8;
        _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(grid, (unsigned short)(i + 0x3c), (unsigned short)i, 0);
        right->flags &= ~8;
        if (det->mapType_ == 1) {
            left->text = det->regular_.nameNoLevel_;
            right->text = det->regular_.levelString_;
        } else if (det->mapType_ == 2) {
            left->text = det->legacy_.mapNameNoLevel_;
            right->text = det->legacy_.mapLevelString_;
        }
        GameState* gs = GameState::GetInstance();
        left->Notify(0xf);
        right->Notify(0xf);
        if (det->discoveryState_ == 1) {
            ClearNodeMaskById_021f6600(obj, i + 0x230, 8);
        } else if (det->discoveryState_ == 2) {
            ClearNodeMaskById_021f6600(obj, i + 0x226, 8);
        } else if (det->discoveryState_ == 3) {
            ClearNodeMaskById_021f6600(obj, i + 0x23a, 8);
        }
        if (CopyToRegion0x6482IfDst((char*)gs, 0)) {
            if (det->discoveryState_ == 1) {
                left->Notify(4);
                right->Notify(4);
            } else if (det->discoveryState_ == 2) {
                left->Notify(4);
                right->Notify(4);
            }
        } else {
            if (!GetAndConsumeNameEntries02011930(gs, 0, 0, 0)) continue;
            if (!data_ov004_02171010.buffer->maps[index].GetInitialByteUnknownBit()) continue;
            left->Notify(4);
            right->Notify(4);
        }
    }
    allocator->Free(scratch);
    BackgroundLoader::RemoveLockGlobal();
    if (data_ov004_02171010.buffer->count == 0) {
        grid->page = 1;
        grid->field_0x5e = 1;
        _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(grid, 0x3c, 0, 0);
        Widget02164084* w = func_ov023_021f6880(func_ov011_021849c8(obj), 0x1f4);
        if (w) w->flags |= 8;
        w = func_ov023_021f6880(func_ov011_021849c8(obj), 0x14);
        if (w) w->flags &= ~4;
        for (unsigned short j = 0; j < 0x10;) {
            w = func_ov023_021f6880(func_ov011_021849c8(obj), j + 0x3c);
            if (w) w->flags |= 8;
            j++;
        }
        w = func_ov023_021f6880(func_ov011_021849c8(obj), 0x36);
        if (w) w->flags &= ~8;
        w = func_ov023_021f6880(ov011, 0x16);
        if (!w) return 0;
        if (func_ov023_021f6f10(w) != 6) return 0;
        w->flags |= 8;
    } else {
        Widget02164084* w = func_ov023_021f6880(func_ov011_021849c8(obj), 0x36);
        if (w) w->flags |= 8;
    }
    func_ov023_021f9d14(grid, obj);
    Widget02164084* w = func_ov023_021f6880(ov011, 0x14);
    if (!w) return 0;
    if (func_ov023_021f6f10(w) != 6) return 0;
    if (data_ov004_02171010.buffer->count <= 8) w->flags &= ~4;
    else w->flags |= 4;
    short page = data_ov004_02171010.buffer->page;
    short field5e = data_ov004_02171010.buffer->field5e;
    w->page = page;
    w->field5e = field5e;
    func_ov023_021f809c(w, obj);
    grid->ready = 1;
    return 0;
}
