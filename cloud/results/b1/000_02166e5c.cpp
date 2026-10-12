#include <globaldefs.h>
#include <Filesystem/BackgroundLoader.h>
#include <GameState/GameState.h>

extern unsigned char data_0211e33c[0x30000] __attribute__((aligned(4)));
void* LoadFileIntoMemory(const char* path, void* buffer, unsigned int* outLength);
unsigned int FindFilesInNarcBySubstring(const void* narcBuffer, const char* substr, const void** pOutFilePtrs, unsigned int* pOutFileSizes, unsigned int maxOutputs);
struct T020545b8;
struct StreamHeader;
struct Struct_203dafc { int words[8]; };
union CombatantLoadInfo02166e5c { Struct_203dafc words; ObjectArchiveLoadInfo archive; };
struct InternalBlock02166e5c { char bytes[0x70]; };
struct Field0x19cData { unsigned char data[0x578]; };
struct CombatantModelState02166e5c { unsigned char data[0x70]; };
struct BattleWork02166e5c {
    char pad_0[0x2a0];
    unsigned char* roster;
    char pad_2a4[0x5734 - 0x2a4];
    Field0x19cData selection;
    CombatantModelState02166e5c model;
};
struct StatTriple02166e5c {
    unsigned int first : 10;
    unsigned int second : 10;
    unsigned int third : 10;
    unsigned int rest : 2;
};
struct ClassEntry02166e5c {
    char name[0x10];
    unsigned char level : 3;
    unsigned char rest_10 : 5;
    unsigned char pad_11;
    unsigned char vocation;
    unsigned char emblem;
    StatTriple02166e5c stats[3];
};
struct ClassTable02166e5c { unsigned char data[0xb8]; };
struct Growth02166e5c {
    unsigned int experience : 32;
    unsigned short vocation : 7;
    unsigned short rank : 9;
    unsigned short pad_6;
    StatTriple02166e5c stats[3];
};
struct CombatantData02166e5c {
    char pad_0[0x3c];
    char name[0x16c - 0x3c];
    unsigned short vocationTable[0xd];
    unsigned char pad_186[0x56a - 0x186];
    unsigned char mode;
    unsigned char level : 4;
    unsigned char rest_56b : 4;
    unsigned char pad_56c[0x94c - 0x56c];
    int phase;
};
struct Combatant02166e5c {
    Object3D object;
    char pad_ac[0xc2 - 0xac];
    unsigned char flags;
    char pad_c3[0x130 - 0xc3];
    PrimaryCombatStats* stats;
    BaseCombatStats* baseStats;
    char* savedStats;
    char pad_13c[0x150 - 0x13c];
    CombatantData02166e5c* data;
};
struct PartyLog02166e5c {
    char pad_0[0xf78];
    unsigned char entries[4];
    unsigned char count;
};
struct RegionCode02166e5c { unsigned char bytes[4]; };

extern "C" GameResources* func_ov017_0218b5b0();
extern "C" T020545b8* _Z24GetIndexedEntry_021a11f4Phi(GameResources*, int);
extern "C" void _Z25DestroyAllocators020545b8P9T020545b8(T020545b8*);
extern "C" void _Z26CopyInternalFields0207df50P11Foo0207df50(InternalBlock02166e5c*);
extern "C" void _Z25RestorePairTables0207df90Pc(char*);
extern "C" void _Z24BackupPairTables0207dfacPc(char*);
void ClearEightWords(Struct_203dafc*);
extern "C" void _Z27ResetSelectionState02054664P11Obj02054664(Combatant02166e5c*);
extern "C" void _Z21SetAndClearField0x19cPcP14Field0x19cData(Combatant02166e5c*, Field0x19cData*);
extern "C" void func_02049c88(Combatant02166e5c*, CombatantModelState02166e5c*);
void RunOverlayScript0216d1c4(int, StreamHeader*, int, int*);
void SetPtrFieldIfNonNull0x13c(void*, int);
void RegisterCombatantSlot(GameState*, int, GameObject*);
extern "C" void _Z35RegisterOrAllocateCombatant02053c74Pci(Combatant02166e5c*, int);
extern "C" CombatantData02166e5c* _Z15GetFieldAt0x150Ph(Combatant02166e5c*);
extern "C" void _Z19ResetStruct020d3c08Pc(ClassTable02166e5c*);
extern "C" void _Z31InitAndMaybeStartStream020d3c28P11Obj020d3c28(ClassTable02166e5c*);
extern "C" ClassEntry02166e5c* _Z15FindEntryByKindP32FindEntryByKindContainer020d3d90i(ClassTable02166e5c*, int);
extern "C" int sprintf(char*, const char*, ...);
extern "C" void func_02083cbc(CombatantData02166e5c*, Growth02166e5c*, void*);
extern "C" void func_02083e28(CombatantData02166e5c*, int);
extern "C" void _Z26SetBit0x954StoreIndex0x950Phi(CombatantData02166e5c*, int);
extern "C" void _Z23ClearTwoRegions02083c18Pc(CombatantData02166e5c*);
extern "C" void _Z18SetBitInArray0x910Phi(CombatantData02166e5c*, int);
extern "C" void _Z17SetBitInField0x34Phi(unsigned char*, int);
extern "C" void _Z20SetByte0xbeShiftPrevP13Bytes02033b88i(Combatant02166e5c*, int);
extern "C" void _Z27CancelPendingAction020397ccP11Obj020397cci(Combatant02166e5c*, int);
extern "C" void _Z26CopyCombatantStats0208936cPcS_S_(char*, PrimaryCombatStats*, BaseCombatStats*);
extern "C" void _Z18CopyInRegion0x571dPciPv(GameState*, int, void*);
void SetCombatWorkFlags0x55f4(void*, int);
extern const char data_ov000_02183aa5[], data_ov000_021839e2[], data_ov000_02183abe[];
extern const char data_020ef078[];
extern const RegionCode02166e5c data_ov000_02183014;

// USA: func_ov000_02166e5c
extern "C" ARM void func_ov000_02166e5c(BattleWork02166e5c* self) {
    InternalBlock02166e5c* blocks;
    GameState* game = GameState::GetInstance();
    GameResources* resources = func_ov017_0218b5b0();
    SafeAllocator* allocator = &resources->allocator_array_38[3];
    blocks = (InternalBlock02166e5c*)resources->unknown_2cc;
    _Z25DestroyAllocators020545b8P9T020545b8(_Z24GetIndexedEntry_021a11f4Phi(resources, 2));
    allocator->Reset();
    _Z26CopyInternalFields0207df50P11Foo0207df50(&blocks[3]);
    _Z25RestorePairTables0207df90Pc(blocks[3].bytes);
    BackgroundLoader::AddLockGlobal();
    unsigned int size;
    if (!LoadFileIntoMemory(data_ov000_02183aa5, data_0211e33c, &size)) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    Combatant02166e5c* combatant = (Combatant02166e5c*)allocator->Allocate(0x2d4);
    if (!combatant) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    CombatantLoadInfo02166e5c loadInfo;
    ClearEightWords(&loadInfo.words);
    loadInfo.archive.fileData = data_0211e33c;
    loadInfo.archive.unk_8 = size;
    loadInfo.archive.allocator = allocator;
    loadInfo.archive.unk_10 = 1;
    _Z27ResetSelectionState02054664P11Obj02054664(combatant);
    if (!combatant->object.LoadFromCCHROrCMOTArchive(&loadInfo.archive, 0)) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    combatant->object.SetScale(0x10a, 0x10a, 0x10a);
    _Z21SetAndClearField0x19cPcP14Field0x19cData(combatant, &self->selection);
    func_02049c88(combatant, &self->model);
    const void* script;
    unsigned int scriptSize;
    if (FindFilesInNarcBySubstring(data_0211e33c, data_ov000_021839e2, &script, &scriptSize, 1)) {
        int scriptState = 0;
        RunOverlayScript0216d1c4((int)allocator, (StreamHeader*)script, scriptSize, &scriptState);
        SetPtrFieldIfNonNull0x13c(combatant, scriptState);
    }
    if (!LoadFileIntoMemory(data_ov000_02183abe, data_0211e33c, &size)) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    ClearEightWords(&loadInfo.words);
    loadInfo.archive.fileData = data_0211e33c;
    loadInfo.archive.unk_8 = size;
    loadInfo.archive.allocator = allocator;
    loadInfo.archive.unk_10 = 1;
    if (!combatant->object.LoadFromCCHROrCMOTArchive(&loadInfo.archive, 0)) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    RegisterCombatantSlot(game, 1, (GameObject*)combatant);
    _Z35RegisterOrAllocateCombatant02053c74Pci(combatant, 1);
    CombatantData02166e5c* data = _Z15GetFieldAt0x150Ph(combatant);
    if (!data) {
        BackgroundLoader::RemoveLockGlobal();
        return;
    }
    ClassTable02166e5c table;
    _Z19ResetStruct020d3c08Pc(&table);
    _Z31InitAndMaybeStartStream020d3c28P11Obj020d3c28(&table);
    ClassEntry02166e5c* entry = _Z15FindEntryByKindP32FindEntryByKindContainer020d3d90i(&table, 1);
    sprintf(data->name, data_020ef078, entry);
    Growth02166e5c growth;
    growth.experience = 0;
    growth.rank = 0;
    growth.vocation = entry->vocation;
    growth.stats[0].first = entry->stats[0].first;
    growth.stats[0].second = entry->stats[0].second;
    growth.stats[0].third = entry->stats[0].third;
    growth.stats[1].first = entry->stats[1].first;
    growth.stats[1].second = entry->stats[1].second;
    growth.stats[1].third = entry->stats[1].third;
    growth.stats[2].first = entry->stats[2].first;
    growth.stats[2].second = entry->stats[2].second;
    growth.stats[2].third = entry->stats[2].third;
    func_02083cbc(data, &growth, 0);
    func_02083e28(data, 0);
    combatant->stats->maxHP = combatant->baseStats->primaryStats.maxHP;
    combatant->stats->maxMP = combatant->baseStats->primaryStats.maxMP;
    _Z26SetBit0x954StoreIndex0x950Phi(data, 0);
    _Z23ClearTwoRegions02083c18Pc(data);
    _Z18SetBitInArray0x910Phi(data, entry->emblem);
    data->vocationTable[0] = entry->vocation;
    data->mode = 0xb;
    data->phase = 3;
    _Z17SetBitInField0x34Phi(self->roster, 1);
    PartyLog02166e5c* log = (PartyLog02166e5c*)GetPtrField0x2a04(game);
    log->entries[log->count++] = 1;
    _Z20SetByte0xbeShiftPrevP13Bytes02033b88i(combatant, 0);
    _Z27CancelPendingAction020397ccP11Obj020397cci(combatant, 1);
    _Z26CopyCombatantStats0208936cPcS_S_(combatant->savedStats, combatant->stats, combatant->baseStats);
    combatant->flags |= 0x20;
    _Z24BackupPairTables0207dfacPc(blocks[3].bytes);
    int level = entry->level;
    CombatantData02166e5c* levelData = combatant->data;
    if (levelData) levelData->level = (unsigned char)level;
    int codeLength = 2;
    RegionCode02166e5c code = data_ov000_02183014;
    _Z18CopyInRegion0x571dPciPv(game, codeLength, &code);
    SetCombatWorkFlags0x55f4(self, 0x400000);
    BackgroundLoader::RemoveLockGlobal();
}
