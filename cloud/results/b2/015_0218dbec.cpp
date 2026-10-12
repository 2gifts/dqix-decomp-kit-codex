#include <globaldefs.h>
#include "std_library_functions.h"
#include "Memory/SafeAllocator.h"
#include "World/Object3D.h"
#include "Filesystem/GPC.h"
#include "Filesystem/FileIO.h"
#include "Filesystem/BackgroundLoader.h"

struct BinarySearchByComparatorStruct;
struct Container02070e60;
struct Foo0207df50;
struct Struct_203dafc;

void* SearchWithComparator0206f4f0(BinarySearchByComparatorStruct*, int);
void* SearchWithLow15Comparator02070fd0(Container02070e60*, int);
void ClearEightWords(struct Struct_203dafc*);

extern "C" void* __clear(void* dst, int count);
extern "C" void _Z26CopyInternalFields0207df50P11Foo0207df50(Foo0207df50*);
extern "C" void _Z25RestorePairTables0207df90Pc(char*);
extern "C" void _Z24BackupPairTables0207dfacPc(char*);
extern "C" void _Z24ResetObjectState02079a3cPv(void*);
extern "C" void _ZN8Object3D10SetField78Et(Object3D*, unsigned int);
extern "C" void _ZN8Object3D10SetField7aEt(Object3D*, unsigned int);
extern "C" void _Z26SetForwardAndStore0205ebc0Pvii(void*, int, int);

struct Record0218dbec {
    int value;
    int number;
    char pad8[0x14 - 8];
    unsigned int packed;
};

struct Entry0218dbec {
    char pad0[8];
    short key;
};

struct Tables0218dbec {
    char pad0[0x64];
    char search64[0xc];
    char search70[0x1a1 - 0x70];
    unsigned char variant;
};

struct Vec3Words02193cf0 { unsigned int v[3]; };

struct Obj0218dbec {
    Tables0218dbec* tables;
    char** pairsRoot;
    SafeAllocator* allocator;
    int selected;
    Record0218dbec* record;
    char pad14[0x24 - 0x14];
    Object3D* object;
    char pad28[0x44 - 0x28];
    int animation;
};

extern char data_ov015_021940f5;
extern char data_ov015_021940fe;
extern char data_ov015_02194105;
extern char data_ov015_0219411d;
extern char data_ov015_02194123;
extern struct Vec3Words02193cf0 data_ov015_02193cf0;
extern int data_02108760;

// USA: func_ov015_0218dbec
extern "C" ARM int func_ov015_0218dbec(Obj0218dbec* self, Entry0218dbec* entry) {
    self->record = (Record0218dbec*)SearchWithComparator0206f4f0((BinarySearchByComparatorStruct*)(self->tables->search64), entry->key);
    if (self->record == 0) {
        return 0;
    }

    char name[0x10];
    __clear(name, 0x10);
    self->allocator->Reset();

    int ok = 1;
    char* mgr = *self->pairsRoot;
    _Z26CopyInternalFields0207df50P11Foo0207df50((Foo0207df50*)mgr);
    _Z25RestorePairTables0207df90Pc(mgr);

    if (self->tables->variant != 0) {
        sprintf(name, &data_ov015_021940f5, self->record->number);
    } else {
        sprintf(name, &data_ov015_021940fe, self->record->number);
    }

    BackgroundLoader::AddLockGlobal();
    BackgroundLoader::FreeAllocationsGlobal();

    GPCReadPair pair;
    _Z24ResetObjectState02079a3cPv(&pair);
    unsigned int headerSize;
    if (LoadAndDecompressGPCHeaderAndInnerFileInfo(&pair.pGPCFile, pair.machine, &data_ov015_02194105,
                                                   data_0211e33c, headerSize, 0x30000, false, 0)) {
        unsigned int fileSize;
        char* narc = (char*)data_0211e33c + headerSize;
        if (DecompressFileFromGPCByName(pair.pGPCFile, pair.machine, narc, fileSize, 0x30000 - headerSize, name)) {
            const void* found = 0;
            unsigned int foundSize = 0;
            unsigned int sizeA = 0;
            unsigned int sizeB = 0;
            void* fileA = 0;
            void* fileB = 0;
            if (FindFilesInNarcBySubstring(narc, &data_ov015_0219411d, &found, &foundSize, 1)) {
                fileA = DecompressLZ77FileIntoAllocatedSpace(*self->allocator, found, sizeA);
            }
            if (FindFilesInNarcBySubstring(narc, &data_ov015_02194123, &found, &foundSize, 1)) {
                fileB = DecompressLZ77FileIntoAllocatedSpace(*self->allocator, found, sizeB);
            }
            ObjectArchiveLoadInfo loadInfo;
            if (fileA != 0) {
                ClearEightWords((Struct_203dafc*)&loadInfo);
                loadInfo.allocator = self->allocator;
                loadInfo.unk_8 = sizeA;
                loadInfo.fileData = fileA;
                loadInfo.unk_10 = 1;
                self->object->LoadFromCCHROrCMOTArchive(&loadInfo, 0);
            }
            if (fileB != 0) {
                ClearEightWords((Struct_203dafc*)&loadInfo);
                loadInfo.allocator = self->allocator;
                loadInfo.unk_8 = sizeB;
                loadInfo.fileData = fileB;
                loadInfo.unk_10 = 1;
                self->object->LoadFromCCHROrCMOTArchive(&loadInfo, 0);
            }
            Vec3Words02193cf0 scale = data_ov015_02193cf0;
            self->object->SetScale((const Vector3fix*)&scale);
            self->object->MaybeSetBCFGAnimation(0, self->animation);
        } else {
            ok = 0;
        }
        pair.Reset();
    } else {
        ok = 0;
    }

    BackgroundLoader::RemoveLockGlobal();
    _Z24BackupPairTables0207dfacPc(mgr);

    int result;
    if (ok == 0) {
        result = 0;
        pair.Reset();
        ZeroDestroyGPCPointer(&pair.pGPCFile);
    } else {
        Record0218dbec* rec = (Record0218dbec*)SearchWithLow15Comparator02070fd0((Container02070e60*)(self->tables->search70), (unsigned short)entry->key);
        if (rec != 0) {
            _ZN8Object3D10SetField78Et(self->object, rec->packed & 0xffff);
            _ZN8Object3D10SetField7aEt(self->object, rec->packed >> 16);
            _Z26SetForwardAndStore0205ebc0Pvii(&data_02108760, rec->packed & 0xffff, rec->packed >> 16);
        }
        self->selected = self->record->value;
        result = 1;
        pair.Reset();
        ZeroDestroyGPCPointer(&pair.pGPCFile);
    }
    return result;
}
