#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/GameResources.h"
#include <std_library_functions.h>

struct ListNode02160094 { char pad0[0x20]; unsigned short id; };
struct List02160094 { unsigned short command; };
struct List02070f98;
struct Struct02070fe4;
struct MonsterDefinition { unsigned short id : 15; unsigned short flag : 1; };
struct DefinitionTable { char pad0[0x400]; unsigned int data[3]; };
struct BattleSelection { char pad0[0x284]; DefinitionTable definitions; };
struct MonsterState {
    char pad0[0x10];
    Vector3i position;
    unsigned char field1c, field1d, field1e, field1f;
    int field20;
    char pad24[0x28];
    unsigned char field4c, field4d;
    char pad4e[0x22];
};
struct MonsterCombatant : GameObject { MonsterState* state; };
ListNode02160094* GetNodeAtIndex02160094(List02160094*, int);
extern "C" BattleSelection* func_ov017_021b8478(void*);
void* GetActiveCombatWork();
int GetElementCount02070fe4(Struct02070fe4*);
MonsterDefinition* GetElementIfInBounds(List02070f98*, int);
extern "C" void func_ov025_021e771c(short*, int, short*, int);
extern "C" void func_ov000_02161c0c(void*, int);
void ClearCombatantSlot(GameState*, int);
extern "C" void func_ov000_021677fc(void*);
void RunFlaglearAndSetMode02167e6c_02167e6c(unsigned char*);
extern short data_ov025_021eef18[];
extern short data_ov025_021eef30[];

// USA: func_ov025_021e7890
extern "C" ARM int func_ov025_021e7890(void*, List02160094* list) {
    int mode;
    unsigned char count;
    int i;
    GameResources* resources = func_ov017_0218b5b0();
    GameState* game = GameState::GetInstance();
    BattleSelection* selection = func_ov017_021b8478(resources->unknown_ptr_3718);
    void* work = GetActiveCombatWork();
    ListNode02160094* node = GetNodeAtIndex02160094(list, 0);
    if (!node) return 1;
    MonsterCombatant* combatant = (MonsterCombatant*)game->GetCombatantByIndex(node->id);
    if (!combatant) return 1;
    if (!combatant->state) return 1;
    MonsterState saved = *combatant->state;
    short ids[10] = {};
    if (list->command == 0x153) {
        memcpy(ids, data_ov025_021eef18, 10);
        mode = 1;
        count = 5;
    } else if (list->command == 0x31a) {
        memcpy(ids, data_ov025_021eef30, 14);
        mode = 3;
        count = 7;
    } else return 1;
    DefinitionTable* definitions = &selection->definitions;
    int size = GetElementCount02070fe4((Struct02070fe4*)definitions->data);
    for (i = 0; i < size; ++i) {
        MonsterDefinition* definition = GetElementIfInBounds((List02070f98*)definitions->data, i);
        if (!definition) continue;
        mode = 1;
        short* id = ids;
        for (int j = 0; j < count; ++j, ++id) {
            if (*id == definition->id) { mode = 0; break; }
        }
        if (mode) ids[count++] = definition->id;
    }
    func_ov025_021e771c(ids, mode, ids, count);
    func_ov000_02161c0c(work, node->id);
    for (int i = 0; i < 8; ++i) {
        short index = i + 0xc0;
        if (node->id != index) ClearCombatantSlot(game, index);
    }
    func_ov000_021677fc(work);
    RunFlaglearAndSetMode02167e6c_02167e6c((unsigned char*)work);
    combatant = (MonsterCombatant*)game->GetCombatantByIndex(node->id);
    if (combatant && combatant->state) {
        MonsterState* state = combatant->state;
        state->position = saved.position;
        state->field1c = saved.field1c;
        state->field1d = saved.field1d;
        state->field1e = saved.field1e;
        state->field1f = saved.field1f;
        state->field20 = saved.field20;
        state->field4c = saved.field4c;
        state->field4d = saved.field4d;
    }
    return 1;
}
