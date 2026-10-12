#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"
#include "Memory/SafeAllocator.h"
#include "std_library_functions.h"

struct CommandArg0215f3fc { int type; int value; };
struct Motion0215f3fc { void* container; char field_4[0x4c]; unsigned int flags; char field_54[0x48]; };
struct Entry0215f3fc { int fields[5]; Motion0215f3fc* motion; void* field_18; void* field_1c; };
struct Name0215f3fc { unsigned short field_0; unsigned char kind; char field_3; char name[16]; };
struct Palette0215f3fc { char data[0x70]; };
struct Scene0215f3fc { char field_0[0x2cc]; Palette0215f3fc palettes[1]; };
struct SlotArray_0203e03c;
struct SlotArray0203dd10;
struct Rec0206bf2c;
struct Obj0203f02c;
struct Node020408c8;
void* GetGlobalPtr021075f4();
Scene0215f3fc* GetWord0x0(int*);
extern "C" const char* func_ov017_021d612c(CommandArg0215f3fc*);
int FindEmptySlotIndex(SlotArray_0203e03c*);
extern "C" void func_ov017_021d6134(CommandArg0215f3fc*, int);
extern "C" unsigned short func_ov017_021d60f4(CommandArg0215f3fc*);
extern "C" char* strstr(const char*, const char*);
extern "C" int sprintf(char*, const char*, ...);
int LoadResourceIntoGlobalBuffer_0215a750(const char*, void**);
void Init020404c0(int*);
void ClearWorkRecord0206bf2c(Rec0206bf2c*);
void InitializeRandomMotion0203c794(void*);
void EmptyStub0203d004();
void SetField0xc(int*, int);
void SetField0x8(int*, int);
void UpdateElementStream0203f02c(Obj0203f02c*, int, int, char*, int, int);
extern "C" void func_0203f138(void*, int);
void SetActiveChildShorts020408c8(Node020408c8*, short, short, short);
void SetSlotIfIndexBelow32(SlotArray0203dd10*, unsigned int, int);
extern char data_ov001_021657ad[];
extern char data_ov001_021657b2[];
extern char data_ov001_021657be[];
extern SafeAllocator* data_ov001_021658b8[];
extern char data_ov001_021657ce[];
extern char data_ov001_021657d1[];

// USA: func_ov001_0215f3fc
extern "C" ARM int func_ov001_0215f3fc(CommandArg0215f3fc* args, int count) {
    GameState* game = GameState::GetInstance();
    void* manager = GetGlobalPtr021075f4();
    Scene0215f3fc* scene = GetWord0x0((int*)game);
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    if (!scene) return 0;
    const char* source = func_ov017_021d612c(args);
    int slot = FindEmptySlotIndex((SlotArray_0203e03c*)manager);
    if (slot == -1) return 0;
    CommandArg0215f3fc* output = args + 1;
    args += 2;
    func_ov017_021d6134(output, slot);
    if (slot >= 32) return 0;
    char filename[80];
    if (strstr(source, data_ov001_021657ad)) sprintf(filename, data_ov001_021657b2, source);
    else sprintf(filename, data_ov001_021657be, source);
    void* file;
    unsigned int fileSize;
    int locked = 0;
    loader->GetLoadedFileByName(filename, &file, &fileSize);
    if (!file) {
        BackgroundLoader::AddLockGlobal();
        locked = 1;
        if (!LoadResourceIntoGlobalBuffer_0215a750(filename, &file)) {
            BackgroundLoader::RemoveLockGlobal();
            return 0;
        }
    }
    int allocatorIndex = 0;
    if (count >= 3) allocatorIndex = func_ov017_021d60f4(args++);
    SafeAllocator* allocator = data_ov001_021658b8[allocatorIndex];
    int paletteIndex = 27;
    if (count >= 4) paletteIndex = func_ov017_021d60f4(args);
    Entry0215f3fc* entry = (Entry0215f3fc*)allocator->Allocate(32);
    if (!entry) { if (locked) BackgroundLoader::RemoveLockGlobal(); return 0; }
    Init020404c0((int*)entry);
    Rec0206bf2c* record = (Rec0206bf2c*)allocator->Allocate(0x78);
    if (!record) { if (locked) BackgroundLoader::RemoveLockGlobal(); return 0; }
    ClearWorkRecord0206bf2c(record);
    Name0215f3fc* name = (Name0215f3fc*)allocator->Allocate(0x14);
    if (!name) { if (locked) BackgroundLoader::RemoveLockGlobal(); return 0; }
    memset(name, 0, 0x14);
    entry->motion = (Motion0215f3fc*)allocator->Allocate(0x9c);
    if (!entry->motion) { if (locked) BackgroundLoader::RemoveLockGlobal(); return 0; }
    InitializeRandomMotion0203c794(entry->motion);
    EmptyStub0203d004();
    if (entry->motion) entry->motion->container = manager;
    name->kind = 3;
    if (strstr(source, data_ov001_021657ad)) {
        int length = 5;
        if (strstr(source, data_ov001_021657ce)) {
            length = 7;
            entry->motion->flags |= 8;
        }
        memcpy(name->name, source, length);
        name->name[length] = 0;
    } else sprintf(name->name, data_ov001_021657d1, source);
    SetField0xc((int*)entry, (int)record);
    SetField0x8((int*)entry, (int)name);
    char* palette = scene->palettes[paletteIndex].data;
    if (entry->motion) UpdateElementStream0203f02c((Obj0203f02c*)entry->motion, (int)&filename[9], (int)allocator, palette, (int)file, fileSize);
    if (entry->motion) func_0203f138(entry->motion, 1);
    if (locked) BackgroundLoader::RemoveLockGlobal();
    SetActiveChildShorts020408c8((Node020408c8*)entry, 0x8f, 0x8f, 0x8f);
    SetSlotIfIndexBelow32((SlotArray0203dd10*)manager, slot, (int)entry);
    return 1;
}

