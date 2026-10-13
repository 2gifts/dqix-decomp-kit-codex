#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/GameResources.h"
#include <std_library_functions.h>

struct List02070f98;
struct Struct02070fe4;
struct MonsterDefinition { unsigned short id : 15; unsigned short flag : 1; };
struct DefinitionTable { char pad0[0x400]; unsigned int data[3]; };
struct BattleSelection { char pad0[0x284]; DefinitionTable definitions; };
struct CameraState { char pad0[0x240]; Vector3i position; };
struct Obj02033874;
extern "C" BattleSelection* func_ov017_021b8478(void*);
void* GetActiveCombatWork();
extern "C" int func_ov000_0215eb1c(void*, short*, int, int);
extern "C" int func_ov000_0215ed34(void*, short*, int);
int AppendMissingTableValues02167484(short*, int);
int GetElementCount02070fe4(Struct02070fe4*);
MonsterDefinition* GetElementIfInBounds(List02070f98*, int);
extern "C" void func_ov025_021e771c(short*, int, short*, int);
extern "C" void func_ov000_02161c0c(void*, int);
void ClearCombatantSlot(GameState*, int);
extern "C" void func_ov000_021677fc(void*);
void RunFlaglearAndSetMode02167e6c_02167e6c(unsigned char*);
extern "C" CameraState* func_ov000_02160f14(void*);
void SetVecYFromValue02033874(Obj02033874*, int);
unsigned char* GetField02163524(void*);
GameObject* GetCombatantWithFlag0x400(GameState*, int);
int GetSubstructByte0x1c(unsigned char*);
extern "C" void func_ov000_02167fb0(void*, GameObject*);
void ResetCombatantSlot020d7334(unsigned char*, int);

// USA: func_ov025_021e7b74
extern "C" ARM int func_ov025_021e7b74(void*, void*, void*, void* battle) {
    int i;
    int retainedCount;
    DefinitionTable* definitions;
    GameResources* resources = func_ov017_0218b5b0();
    GameState* game = GameState::GetInstance();
    BattleSelection* selection = func_ov017_021b8478(resources->unknown_ptr_3718);
    void* work = GetActiveCombatWork();
    short retainedIds[8];
    retainedCount = (unsigned char)func_ov000_0215eb1c(battle, retainedIds, 8, 0x10);
    short ids[12];
    unsigned char originalCount = func_ov000_0215ed34(battle, ids, 12);
    unsigned char count = AppendMissingTableValues02167484(ids, originalCount);
    definitions = &selection->definitions;
    int size = GetElementCount02070fe4((Struct02070fe4*)definitions->data);
    for (i = 0; i < size; ++i) {
        MonsterDefinition* definition = GetElementIfInBounds((List02070f98*)definitions->data, i);
        if (!definition) continue;
        i = 1;
        short* id = ids;
        for (int j = 0; j < count; ++j, ++id) {
            if (*id == definition->id) { i = 0; break; }
        }
        if (i) ids[count++] = definition->id;
    }
    func_ov025_021e771c(ids, originalCount, ids, count);
    unsigned char retained[8];
    memset(retained, 0, sizeof(retained));
    short* retainedId = retainedIds;
    for (int i = 0; i < retainedCount; ++i) {
        int id = *retainedId++;
        retained[id - 0xc0] = 1;
        func_ov000_02161c0c(work, id);
    }
    unsigned char* retainedSlot = retained;
    for (short id = 0xc0; id <= 0xc7; ++id, ++retainedSlot) {
        if (!*retainedSlot) ClearCombatantSlot(game, id);
    }
    func_ov000_021677fc(work);
    RunFlaglearAndSetMode02167e6c_02167e6c((unsigned char*)work);
    Vector3i cameraPosition = func_ov000_02160f14(work)->position;
    int cameraZ = cameraPosition.z;
    for (int id = 0xc0; id < 0xc8; ++id) {
        GameObject* actor = game->GetCombatantByIndex(id);
        if (!actor) continue;
        Vector3i target = {};
        target.z = cameraZ;
        Vector3i position = actor->obj3D_.position_;
        position.y = 0;
        Vector3i direction;
        Vector3fix_Subtract(&target, &position, &direction);
        direction.y = 0;
        Vector3fix_Normalize(&direction, &direction);
        SetVecYFromValue02033874((Obj02033874*)actor, fix32_Atan2(direction.x, direction.z));
    }
    unsigned char* slots = GetField02163524(work);
    retainedId = retainedIds;
    for (int i = 0; i < retainedCount; ++retainedId, ++i) {
        GameObject* actor = GetCombatantWithFlag0x400(game, *retainedId);
        if (actor && GetSubstructByte0x1c((unsigned char*)actor) == 0xff) {
            func_ov000_02167fb0(work, actor);
            ResetCombatantSlot020d7334(slots, *retainedId);
        }
    }
    return 1;
}
