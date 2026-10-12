#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/NarcHandle.h"
#include "Filesystem/FileIO.h"
#include "Filesystem/FileAccessor.h"
#include "Filesystem/LowNitroHandle.h"
#include "Grotto/Main/ActiveGrottoClass.h"
#include "World/ZoneFeatures.h"
#include "std_library_functions.h"

extern "C" char* func_02012fe4(void);
extern "C" void func_020a0cc4(unsigned int size);
extern "C" void func_020a0c0c(void);
extern "C" int _Z17IsInRange0201b5b0i(int id);
extern "C" void func_02019728(char* zone, int floor, ZoneFeatures* features, void* buffer, FloorMap* map);
extern "C" void func_0201a300(char* zone, int floor, void* monster, void* buffer, FloorMap* map);
void* AllocateAligned4(AllocatorUnion* alloc, unsigned int size);
void TailForward02012da4(AllocatorUnion* alloc, void* data);

extern AllocatorUnion data_02114e20;
extern const char data_ov017_021d7b53[];
extern const char data_ov017_021d7b57[];
extern const char data_ov017_021d7b3a[];

struct Monster021b4a88 {
    unsigned short id;
    unsigned char pad2[0xc - 2];
    unsigned char loaded;
};

struct Request021b4a88 {
    unsigned char pad0[8];
    Monster021b4a88* monster;
    int taskId;
    unsigned char state;
};

// USA: func_ov017_021b4a88
extern "C" ARM void func_ov017_021b4a88(Request021b4a88* request) {
#if defined(jpn)
 enum {regionalOffset0=0x240c};
#else
 enum {regionalOffset0=0x23ec};
#endif
    GameState::GetInstance();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    char* zone = func_02012fe4();
    ActiveGrottoClass* grotto = (ActiveGrottoClass*)(zone + regionalOffset0);
    ZoneFeatures features;
    features.Reset();

    if (loader->GetTaskStatus(request->taskId)) {
        if (loader->GetDetailedTaskStatus(request->taskId) == BackgroundLoader::TaskStatus_Complete) {
            unsigned int size;
            void* data;
            loader->GetLoadedFileByID(request->taskId, &data, &size);
            func_020a0cc4(0x2800);

            SafeAllocator scriptAllocator;
            scriptAllocator.ResetAllocatorPointer();
            scriptAllocator.CreateTypeA(AllocateAligned4(&data_02114e20, 0x800), 0x800);
            scriptAllocator.Reset();

            NarcHandle archive;
            if (archive.Initialize(data_ov017_021d7b53, (const unsigned char*)data)) {
                NitroVM vm;
                NitroVM_Initialize(&vm);
                char path[0x50];
                unsigned int index = 0;
                while (PrepareReadFileInNARCByID(&vm, &archive, index)) {
                    NitroVM_WriteOutFilePath(&vm, path, 0x50);
                    char* extension = strrchr(path, '.');
                    if (extension == 0) {
                        NitroVM_FinishRead(&vm);
                        index++;
                        continue;
                    }
                    NitroVM_FinishRead(&vm);
                    const void* file = archive.GetFileByIndex(index);
                    if (strcmp(data_ov017_021d7b57, extension) == 0) {
                        unsigned int scriptSize;
                        void* script = DecompressLZ77FileIntoScratchSpace(scriptAllocator, file, scriptSize);
                        features.Reset();
                        features.LoadFromScript(&scriptAllocator, script, scriptSize);
                    }
                    index++;
                }
                archive.Destroy();
            }

            int id = request->monster->id;
            request->monster->loaded = 1;
            int floor = id % 20;
            SafeAllocator floorAllocator;
            floorAllocator.ResetAllocatorPointer();
            floorAllocator.CreateTypeA(AllocateAligned4(&data_02114e20, 0x600), 0x600);
            floorAllocator.Reset();
            if (_Z17IsInRange0201b5b0i(id)) {
                int width = grotto->CalculateAndStoreFloorWidth(floor);
                int height = grotto->CalculateAndStoreFloorHeight(floor);
                ActiveGrottoClass* current = (ActiveGrottoClass*)(zone + regionalOffset0);
                current->ClearGenerator(true);
                current->AllocateGenerator(&floorAllocator, true);
                FloorMap map;
                map.Clear();
                map.Clear();
                map.AllocateBuffers(&floorAllocator);
                void* buffer = AllocateAligned4(&data_02114e20, 0x1000);
                current->CalculateFloorMap(floor, width, height, &map);
                func_02019728(zone, floor, &features, buffer, &map);
                func_0201a300(zone, floor, request->monster, buffer, &map);
                TailForward02012da4(&data_02114e20, buffer);
                map.Clear2();
            }
            void* scriptHeap = scriptAllocator.GetSignedAllocator();
            scriptAllocator.Destroy();
            TailForward02012da4(&data_02114e20, scriptHeap);
            void* floorHeap = floorAllocator.GetSignedAllocator();
            floorAllocator.Destroy();
            TailForward02012da4(&data_02114e20, floorHeap);
            func_020a0c0c();
        }
        loader->RemoveTask(request->taskId);
        request->taskId = -1;
        request->taskId = loader->QueueLoadFile(data_ov017_021d7b3a, 0);
        request->state = 3;
    }
    features.Reset();
}
