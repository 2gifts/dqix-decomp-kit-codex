#include <globaldefs.h>

#include "Combat/Main/BattleList.h"
#include "Combat/ResourceScriptEntry.h"
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/FileIO.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"
#include "std_library_functions.h"

struct StreamHeader;

extern "C" void* func_ov011_021845f8(void* ctx, int v);
unsigned char CopyOutRegion0x571d(char* obj, void* dst);
int GetFieldAt0x150(unsigned char* obj);
extern "C" void* __clear(void* dst, int count);
extern "C" int _Z24InitAndRunScript0208274cPvP12StreamHeaderii(void* ctx, StreamHeader* script, int size, int arg3);

struct Record021707e8 {
    ResourceScriptPackedTriple first;
    ResourceScriptPackedTriple second;
    ResourceScriptPackedTriple third;
    unsigned int fourth : 10;
    unsigned int rest : 22;
    unsigned int pad[2];
};

struct State021707e8 {
    unsigned char active;
    unsigned char pad1[3];
    unsigned char pending;
    unsigned char pad5[0xf];
    Record021707e8* records;
};

extern State021707e8 data_ov004_021707e8;
extern int data_ov004_02170854[];
extern char data_ov004_0217052d[];
extern unsigned char data_0211e33c[0x30000];

static inline int TableNumber(int index) {
    int number = data_ov004_02170854[index];
    return number;
}

// USA: func_ov004_02162dd8
extern "C" ARM int func_ov004_02162dd8(void* a) {
    GameState* gs;
    unsigned char ids[4];
    unsigned int size;
    char path[0x20];
    ResourceScriptEntry entry;

    if (data_ov004_021707e8.pending == 0) return 0;
    data_ov004_021707e8.active = 1;

    gs = GameState::GetInstance();
    int count = CopyOutRegion0x571d((char*)gs, ids);
    data_ov004_021707e8.records = 0;
    SafeAllocator* alloc = (SafeAllocator*)((char*)func_ov011_021845f8(a, 4) + 4);
    Record021707e8* records = (Record021707e8*)alloc->Allocate(0x480);
    data_ov004_021707e8.records = records;
    if (records != 0) {
        memset(records, 0, 0x480);
        BackgroundLoader::AddLockGlobal();
        for (int i = 0; i < count; i++) {
            for (long j = 1; j < 13; j++) {
                __clear(path, sizeof(path));
                sprintf(path, data_ov004_0217052d, data_ov004_02170854[j]);
                if (LoadFileIntoMemory(path, data_0211e33c, &size) == 0) continue;
                GameObject* combatant = GetCombatantWithFlag0x100(gs, ids[i]);
                if (combatant == 0) continue;
                unsigned char* field = (unsigned char*)GetFieldAt0x150((unsigned char*)combatant);
                if (field == 0) continue;
                _Z24InitAndRunScript0208274cPvP12StreamHeaderii(&entry, (StreamHeader*)data_0211e33c, size,
                                                                 *(unsigned short*)(field + data_ov004_02170854[j] * 2 + 0x16c));
                Record021707e8* rec = &data_ov004_021707e8.records[TableNumber(j) - 1 + ids[i] * 12];
                rec->first.low = entry.initial.value8.low;
                rec->first.middle = entry.initial.value8.middle;
                rec->first.high = entry.initial.value8.high;
                rec->second.low = entry.initial.valueC.low;
                rec->second.middle = entry.initial.valueC.middle;
                rec->second.high = entry.initial.valueC.high;
                rec->third.low = entry.initial.value10.low;
                rec->third.middle = entry.initial.value10.middle;
                rec->third.high = entry.initial.value10.high;
                rec->fourth = entry.initial.valueC.middle;
            }
        }
        BackgroundLoader::RemoveLockGlobal();
    }
    data_ov004_021707e8.pending = 0;
    data_ov004_021707e8.active = 0;
    return 0;
}
