#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/GPC.h"
#include "GameState/GameState.h"
#include "Filesystem/FileIO.h"

struct Struct0200fb08;
struct FlagWord02046708;
int NormalizeField5_0200fb08(Struct0200fb08*);
FlagWord02046708* GetDataPtr02114e04_020d6c00();
int TestFlags02046708(FlagWord02046708*, unsigned int);
extern "C" void __clear(void*, unsigned int);
extern unsigned char data_0211e33c[] __attribute__((aligned(4)));

// USA: ExtractFileFromGP2
extern "C" ARM void* ExtractFileFromGP2(const char* archive, const char* filename, unsigned int* length) {
    if (length != NULL) *length = 0;
    BackgroundLoader::FreeAllocationsGlobal();
    GameState* state = GameState::GetInstance();
    char localized[0x80];
    __clear(localized, sizeof(localized));
    StringReplaceLanguageTag(filename, localized, NormalizeField5_0200fb08((Struct0200fb08*)state));
    unsigned int headerLength = 0;
    unsigned char* buffer = data_0211e33c;
    unsigned int capacity = 0x30000;
    if (TestFlags02046708(GetDataPtr02114e04_020d6c00(), 0x02000000)) capacity -= 0x8000;
    GPCReadPair pair;
    GPCReadPair* reader = &pair;
    ZeroInitGPCPointer(&reader->pGPCFile);
    reader->ZeroInitializeMachine();
    if (LoadAndDecompressGPCHeaderAndInnerFileInfo(&reader->pGPCFile, reader->machine, archive, buffer, headerLength, capacity, false, NULL)) {
        unsigned int offset = headerLength;
        unsigned int decompressed = 0;
        DecompressFileFromGPCByName(reader->pGPCFile, reader->machine, buffer + offset, decompressed, capacity - offset, localized);
        *length = decompressed;
        reader->Reset();
        ZeroDestroyGPCPointer(&reader->pGPCFile);
        return buffer + offset;
    }
    reader->Reset();
    ZeroDestroyGPCPointer(&reader->pGPCFile);
    return NULL;
}
