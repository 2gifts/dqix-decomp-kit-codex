#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Util/Random.h"

struct ZeroableStruct021620d8 {
    unsigned char next;
    unsigned char field1;
    unsigned short species;
    int used;
};
struct FormationEntry {
    unsigned short species;
    unsigned char index;
    unsigned char padding3[7];
    unsigned char width : 4;
    unsigned char height : 4;
    unsigned char paddingb[13];
};
struct Formation {
    unsigned char width;
    unsigned char height : 4;
    unsigned char flags : 2;
    unsigned char count : 2;
    unsigned char padding2[2];
    FormationEntry entries[3];
    ZeroableStruct021620d8 species[4];
};
struct BattleRandom {
    unsigned char padding0[0x81b0];
    Formation formation;
};
struct BinarySearchByComparatorStruct;
struct Container02070e60;
struct Param02048850;
struct Obj02048850;
struct FieldStruct02048934 {
    unsigned char padding0[0x2c];
    unsigned char parameters[0x34];
};
struct EnemyLookup {
    unsigned char padding0[0x400];
    unsigned char parameters[0xc];
    unsigned char names[0xc];
};
struct EnemyData {
    union {
        struct { unsigned char padding0[0x278]; EnemyLookup lookup; };
        struct { unsigned char padding_names[0x684]; unsigned char names[0xc]; };
    };
};
struct BattleState {
    unsigned char padding0[0x29c];
    BattleRandom* random;
    EnemyData* enemies;
};
struct EnemyStats {
    PrimaryCombatStats primary;
    unsigned char padding14[0x11];
    unsigned char slot;
};
struct EnemyCombatant {
    Object3D object;
    unsigned char paddingac[0x8c];
    EnemyStats* stats;
    unsigned char padding13c[0xc];
    FieldStruct02048934* species;
    unsigned char padding14c[0x30];
    unsigned char formationIndex;
    unsigned char padding17d[7];
    unsigned char active;
    unsigned char padding185;
    unsigned short speciesId;
    unsigned short hp;
    unsigned short mp;
    unsigned char groupIndex;
    unsigned char letter;
};
extern "C" void* func_0202ae18();
extern "C" void func_020891cc(EnemyStats*);
extern "C" void func_02089630(EnemyStats*, void*, int);
extern "C" int func_ov000_02161bb8(ZeroableStruct021620d8*);
FieldStruct02048934* SearchWithLow15Comparator02070fd0(Container02070e60*, int);
Param02048850* SearchWithComparator0206f4f0(BinarySearchByComparatorStruct*, int);
EnemyCombatant* GetCombatantWithFlag0x400(GameState*, int);
void ZeroStruct021620d8(ZeroableStruct021620d8*);
int FindOrAssignSlot02167c28(unsigned char*, int);
void SetupFieldsFromParam02048850(Obj02048850*, Param02048850*);
void SetRandomizedShort0x18802048934(void*, FieldStruct02048934*, Random*);
void IncrementByteCounterWrapAt0x12(unsigned char*);

// USA: func_ov000_02161e30
extern "C" ARM void func_ov000_02161e30(BattleState* state, int combatantId, int speciesId) {
    FieldStruct02048934* species;
    EnemyLookup* lookup;
    EnemyCombatant* combatant;
    Formation* formation = &state->random->formation;
    GameState* game = GameState::GetInstance();
    func_0202ae18();
    lookup = &state->enemies->lookup;
    species = SearchWithLow15Comparator02070fd0((Container02070e60*)state->enemies->names, speciesId);
    Param02048850* parameters = SearchWithComparator0206f4f0((BinarySearchByComparatorStruct*)lookup->parameters, (short)speciesId);
    if (!species || !parameters) return;
    combatant = GetCombatantWithFlag0x400(game, combatantId);
    int index = combatant->formationIndex;
    formation->width -= formation->entries[index].width - 1;
    formation->height = (unsigned char)(formation->height - (formation->entries[index].height - 1));
    memset(&formation->entries[index], 0, sizeof(FormationEntry));
    formation->entries[index].height = 1;
    formation->entries[index].width = 1;
    formation->entries[index].species = speciesId;
    formation->entries[index].index = combatantId - 0xc0;
    int found = 0;
    int slot = 0;
    for (int i = 0; i < formation->count; i++) {
        if (speciesId == formation->species[i].species) {
            slot = i;
            found = 1;
            break;
        }
    }
    if (!found) {
        slot = formation->count;
        formation->count = (unsigned char)(formation->count + 1);
        ZeroStruct021620d8(&formation->species[slot]);
        formation->species[slot].species = speciesId;
        formation->species[slot].next = 1;
    }
    FindOrAssignSlot02167c28((unsigned char*)state, combatantId);
    EnemyStats* stats = combatant->stats;
    func_020891cc(stats);
    func_02089630(stats, species->parameters, 0);
    combatant->species = species;
    int height = combatant->object.GetHeight();
    int radius = combatant->object.GetRadius();
    SetupFieldsFromParam02048850((Obj02048850*)combatant, parameters);
    combatant->object.SetHeight(height);
    combatant->object.SetRadius(radius);
    SetRandomizedShort0x18802048934(combatant, species, (Random*)state->random);
    unsigned short hp = combatant->hp;
    unsigned short mp = combatant->mp;
    combatant->stats->primary.currHP = hp;
    combatant->stats->primary.maxHP = hp;
    combatant->stats->primary.currMP = mp;
    combatant->stats->primary.maxMP = mp;
    ZeroableStruct021620d8* letterGroup = &formation->species[slot];
    unsigned char letter = func_ov000_02161bb8(letterGroup);
    combatant->speciesId = speciesId;
    if (letter == 1) letter = 0;
    combatant->groupIndex = index;
    combatant->letter = letter;
    combatant->active = 1;
    formation->species[slot].used |= 1 << combatant->stats->slot;
    IncrementByteCounterWrapAt0x12((unsigned char*)letterGroup);
}
