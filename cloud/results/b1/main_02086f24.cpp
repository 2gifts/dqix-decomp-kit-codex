#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/FileIO.h"
#include "Memory/SafeAllocator.h"
#include "System/Memory.h"
#include "std_library_functions.h"

struct StreamHeader;
struct Ctx0209a8b4 { void* field0; };
struct Holder0209ab20;

struct ResetFieldsStruct {
    int f0;
    short f4;
    short f6;
    short f8;
    char pad0a[2];
    int fc;
    short f10;
    char pad12[2];
    int f14;
    short f18;
};

struct Entry02089fb0 {
    signed char id;
    const char* name;
    unsigned char flag08;
    short field0a;
    unsigned char pad0c[4];
    unsigned char mode10;
    unsigned char field11;
    unsigned char field12;
    float scaleX;
    float scaleY;
    short params[10];
    unsigned char enabled[13];
    unsigned char slot;
    unsigned char count;
    unsigned char cursor;

    short GetParam(int i) const { short param = params[i]; return param; }
};
struct EntryArray02089fb0 {
    struct Entry02089fb0* base;
    short count;
};

struct PackedTriple02086f24 {
    unsigned int x : 10;
    unsigned int y : 10;
    unsigned int z : 10;
    unsigned int reserved : 2;
};
struct Obj02082cdc { PackedTriple02086f24 values[3]; };
struct Obj020827c4 {
    unsigned char params[0x14];
    unsigned char flag : 1;
    unsigned char mode : 3;
    unsigned char level : 4;
    unsigned char kind : 4;
    unsigned char : 4;
    short field16;
    short scaleX;
    short scaleY;
};
struct CombatSelection {
    signed char selected : 6;
    unsigned char active : 1;
    unsigned char reserved : 1;
    unsigned char count : 4;
    unsigned char ready : 1;
    unsigned char mode : 3;
    unsigned char enabled[13];
    unsigned char status[13];
    int entries[13];
    int current;
    unsigned short selectionMask;
    char pad56[2];
    Obj02082cdc triples[13];
    unsigned short fieldf4;
    unsigned char flags[27];
    unsigned char groupFlags[9];
    unsigned char commandFlags[36];
    unsigned char field13e;
    unsigned char cursor : 3;
    unsigned char capacity : 5;
    unsigned char name[12];
    PackedTriple02086f24 vectors[4];
    unsigned int timer : 10;
    unsigned int timerLimit : 10;
    unsigned int reserved15c : 12;
    Obj020827c4 timers;
    unsigned char history[0xc0];
};

struct Model02086f24 { unsigned char data[0x20]; };
struct CombatantState02086f24 {
    PackedTriple02086f24 packed[6];
    unsigned char pad18[0x54];
    unsigned short field6c;
    unsigned short field6e;
    unsigned short field70;
    unsigned short field72;
    unsigned char pad74[0x120];
    struct Model02086f24 models[11];
    unsigned char pad2f4[0x670];
};

struct ResourceEntry02086f24 {
    unsigned int id;
    unsigned short low : 7;
    unsigned short high : 9;
    unsigned char rest[0x4e];
};

struct Archive02086f24 {
    int words[5];
    unsigned char field14;
};

struct Party02086f24 {
    char pad[0xf70];
    unsigned int joinCount;
};

void ResetStructFields(struct ResetFieldsStruct* s);
void ForwardResetStructFields02089ddc(struct ResetFieldsStruct* s);
void SetupHeaderAndRunBufferedScript02089de8(void* param0, void* param1, const char* param2);
struct Entry02089fb0* FindEntryBySignedId02089fb0(struct EntryArray02089fb0* arr, int id);
extern "C" void func_02086404(struct CombatSelection* selection);
extern "C" void __clear(void* buf, int n);
extern "C" int func_020426bc(const char* str, unsigned char* out, int id);
extern "C" void func_02082828(struct CombatantState02086f24* state);
extern "C" void func_02082d6c(struct CombatantState02086f24* state, struct CombatSelection* selection);
void LoadAndApplyResourceEntry02082fc4(void* a, void* b);
void* ZeroInitReturn020de824(void* obj);
void ZeroInit020de848(void* obj);
int InitAndDispatch020def44(int a0, int a1, int a2, short a3);
extern "C" void func_02083e28(void* model, int arg);
void ClearField00209a804(int* field);
void SetupAndRunBufferedScript0209a8b4(struct Ctx0209a8b4* ctx, SafeAllocator* alloc, struct StreamHeader* buffer, int length);
int CollectPairsBelowThreshold0209ab20(struct Holder0209ab20* h, int slot, int threshold, unsigned char* out);
extern "C" int func_02086778(void* map, struct CombatSelection* buf, int c, int d);

extern const char* data_020f2a38;
extern const char* data_020f2a30;
extern signed char data_020e8a94[10];
extern char data_020f102e[];
extern unsigned char data_0211e33c[0x30000];

// USA: func_02086f24
ARM int JoinPartyMember02086f24(struct Party02086f24* party, int id, SafeAllocator* heap) {
    struct CombatSelection selection;
    struct ResourceEntry02086f24 resource;
    struct CombatantState02086f24 state;
    struct ResetFieldsStruct fields;
    SafeAllocator alloc;
    unsigned char name[12];
    struct Archive02086f24 archive;
    unsigned char list[0x14];
    unsigned int archiveSize;
    unsigned int size;
    Ctx0209a8b4 ctx;
    struct Entry02089fb0* entry;
    unsigned char slot;
    int result;

    if (heap->GetMaxPossibleAllocation() < 0x800) {
        return 0;
    }
    alloc.ResetAllocatorPointer();
    alloc.ResetAllocatorPointer();
    alloc.CreateTypeA(heap->Allocate(0x800), 0x800);
    alloc.Reset();
    ResetStructFields(&fields);
    SetupHeaderAndRunBufferedScript02089de8(&fields, &alloc, 0);
    entry = FindEntryBySignedId02089fb0((struct EntryArray02089fb0*)&fields, (short)(id + 7));
    func_02086404(&selection);
    slot = 0;
    if (entry != NULL) {
        selection.active = 1;
        selection.count = entry->count;
        selection.cursor = entry->cursor;
        int memberSlot = entry->slot;
        selection.selectionMask |= 1 << memberSlot;
        selection.current = memberSlot;
        __clear(name, sizeof(name));
        func_020426bc(entry->name, name, 1);
        memcpy(selection.name, name, sizeof(name));
        VectorizedInvertedMemcpy(entry->enabled, selection.enabled, sizeof(selection.enabled));
        struct Obj020827c4* timers = &selection.timers;
        VectorizedInvertedMemcpy(entry->params, timers, sizeof(entry->params));
        timers->flag = entry->flag08;
        timers->mode = entry->mode10;
        timers->level = entry->field12;
        timers->kind = entry->field11;
        timers->field16 = entry->field0a;
        timers->scaleX = (short)(entry->scaleX > 0.0f ? (short)(entry->scaleX * 4096.0f + 0.5f) : (short)(entry->scaleX * 4096.0f - 0.5f));
        timers->scaleY = (short)(entry->scaleY > 0.0f ? (short)(entry->scaleY * 4096.0f + 0.5f) : (short)(entry->scaleY * 4096.0f - 0.5f));
        func_02082828(&state);
        func_02082d6c(&state, &selection);
        LoadAndApplyResourceEntry02082fc4(&state, &resource);
        BackgroundLoader::AddLockGlobal();
        archiveSize = 0;
        ZeroInitReturn020de824(&archive);
        ZeroInit020de848(&archive);
        void* data = ExtractFileFromGP2(data_020f2a38, data_020f2a30, &archiveSize);
        for (int i = 0; i < 10; i++) {
            InitAndDispatch020def44((int)&state.models[data_020e8a94[i]], (int)data, archiveSize, entry->GetParam(i));
        }
        BackgroundLoader::RemoveLockGlobal();
        func_02083e28(&state, 0);
        selection.vectors[0].x = state.packed[0].x;
        selection.vectors[0].y = state.packed[0].y;
        selection.vectors[0].z = state.packed[0].z;
        selection.vectors[1].x = state.packed[1].x;
        selection.vectors[1].y = state.packed[1].y;
        selection.vectors[1].z = state.packed[1].z;
        selection.vectors[2].x = state.packed[2].x;
        selection.vectors[2].y = state.field6c;
        selection.vectors[3].x = state.field6c & 0x3ff;
        selection.vectors[2].z = state.field6e;
        selection.vectors[3].y = state.field6e;
        selection.vectors[3].z = state.field70;
        selection.timer = state.field72;
        selection.timerLimit = state.packed[3].x;
        selection.fieldf4 = resource.high;
        selection.entries[entry->slot] = resource.id;
        slot = entry->slot;
    } else {
        result = 0;
    }
    ForwardResetStructFields02089ddc(&fields);
    alloc.Reset();
    if (result != 0) {
        struct StreamHeader* script = (struct StreamHeader*)LoadFileIntoMemory(data_020f102e, data_0211e33c, &size);
        if (script != NULL) {
            ClearField00209a804((int*)&ctx);
            SetupAndRunBufferedScript0209a8b4(&ctx, &alloc, script, size);
            int n = CollectPairsBelowThreshold0209ab20((struct Holder0209ab20*)&ctx, slot, selection.enabled[slot], list);
            unsigned char* p = list;
            for (int i = 0; i < n; i++) {
                int bit = *p++;
                selection.groupFlags[bit / 8] |= 1 << (bit % 8);
            }
        }
    }
    if (result != 0) {
        result = func_02086778(party, &selection, 1, 0);
        if (++party->joinCount > 4) {
            party->joinCount = 4;
        }
    }
    alloc.Destroy();
    return result;
}
