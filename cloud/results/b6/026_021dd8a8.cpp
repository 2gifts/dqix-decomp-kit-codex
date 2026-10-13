#include <globaldefs.h>
#include "GameState/GameState.h"

struct S_10088;
struct FlagObj_021dd3c4;
struct TextCombatant { int data[2]; unsigned int flags; };
struct CombatantStatus { unsigned int flags; };
struct CombatantView {
    char pad0[0x130];
    CombatantStatus* status;
    char pad134[0x148 - 0x134];
    void* nameData;
};
struct EnemyGroup {
    unsigned short species;
    unsigned char ids[8];
    unsigned char count : 4;
    unsigned char reserved : 4;
    char padb[0x18 - 0xb];
};
struct EnemyGroups {
    unsigned char field81b0;
    unsigned char kind : 4;
    unsigned char groupCount : 2;
    unsigned char reserved : 2;
    char pad81b2[2];
    EnemyGroup groups[3];
};
struct BattleState {
    char pad0[0x81b0];
    EnemyGroups enemies;
    char pad81fc[0x8e24 - 0x81fc];
    int turn;
    char pad8e28[0x8e49 - 0x8e28];
    unsigned char mode;
};
struct PartyState { char pad0[8]; unsigned short side; };
struct StoreStruct {
    char pad0[0x20];
    TextCombatant* descriptions[3];
    char pad2c[4];
    unsigned char selectedCount;
    unsigned char aliveCount;
    char pad32[0x5c - 0x32];
    char* buffer;
    char pad60[0x998 - 0x60];
    int active;
    char pad99c[0x195b - 0x99c];
    unsigned char flags;
    char pad195c[0x19b1 - 0x195c];
    unsigned char field19b1;
    unsigned char field19b2;
    char pad19b3[0x19d7 - 0x19b3];
    unsigned char plural;
};
struct Container_021dae60 { char pad0[0x955]; unsigned char side; };
struct Encounter { char pad0[0xc]; int id; };

StoreStruct* GetGlobalField0x1c020421a0();
GameObject* GetCombatantWithFlag0x400(GameState*, int);
int IsFlag10088Set(S_10088*);
int IsFlagBit8192Set_021dd3c4(FlagObj_021dd3c4*);
GameResources* func_ov017_0218b5b0();
void StoreInArray0x8b0(StoreStruct*, int, int);
int CopyOutRegion0x571d(char*, void*);
void* GetActiveCombatWork();
Container_021dae60* GetOffsetPtr02160f08(void*);
void* FindSlotById_021dae60(Container_021dae60*, int);
int TestBitAt0x34(unsigned char*, unsigned int);
char* InitAndClearBuffer_021d8a78(void*);
extern "C" {
void func_02046380(StoreStruct*);
void func_020e4ce8(TextCombatant*, GameObject*, int);
Encounter* func_ov017_021b8478(void*, int);
void func_020dd0b0(int, char*);
void* func_ov017_021b8468(void*);
void func_0204500c(StoreStruct*, char*, int, int);
void func_02042b98(StoreStruct*, int, int, int, int);
}
extern signed char data_ov026_021de6a6[2];
extern signed char data_ov026_021de6a8[];
extern signed char data_ov026_021de6a2[4];
extern char data_ov026_021deebd[];
extern char data_ov026_021deec7[];
extern char data_ov026_021deed8[];

static inline char* EncounterMessage(int id) {
    StoreStruct* work = GetGlobalField0x1c020421a0();
    char* buffer = work->buffer;
    memset(buffer, 0, 0x960);
    func_020dd0b0(id, buffer);
    return buffer;
}

// USA: func_ov026_021dd8a8
extern "C" ARM void func_ov026_021dd8a8(BattleState* state, PartyState* party) {
    GameState* game = GameState::GetInstance();
    StoreStruct* work = GetGlobalField0x1c020421a0();
    if (state->turn != 0) return;
    func_02046380(work);
    int distinct;
    EnemyGroup* group;
    int i, k;
    EnemyGroups* enemies = &state->enemies;
    distinct = 0;
    unsigned short species[3];
    unsigned char duplicate[3] = {};
    unsigned char counts[3] = {};
    TextCombatant descriptions[3];
    char text[0x300];
    unsigned char ids[4];
    group = enemies->groups;
    for (i = 0; i < enemies->groupCount; ++group, ++i) {
        int found = 0;
        for (int j = 0; j < distinct; ++j) {
            if (group->species == species[j] || group->species == 0) {
                if (group->species == species[j]) duplicate[j] = 1;
                found = 1;
                break;
            }
        }
        if (!found) {
            species[distinct] = group->species;
            for (k = 0; k < group->count; ++k) {
                GameObject* object = GetCombatantWithFlag0x400(game, group->ids[k] + 0xc0);
                if (!object || IsFlag10088Set((S_10088*)object) ||
                    IsFlagBit8192Set_021dd3c4((FlagObj_021dd3c4*)object) ||
                    !((CombatantView*)object)->nameData) continue;
                TextCombatant* description = &descriptions[distinct];
                func_020e4ce8(description, object, 1);
                description->flags &= ~0x80000000;
                work->descriptions[distinct] = description;
                counts[distinct] = group->count;
                ++distinct;
                break;
            }
        }
    }
    unsigned char selected;
    unsigned char alive;
    GameResources* resources = func_ov017_0218b5b0();
    void* context = 0;
    if (resources) context = resources->unknown_ptr_3718;
    Encounter* encounter = 0;
    int encounterId = -1;
    if (context) encounter = func_ov017_021b8478(context, 0);
    if (encounter) encounterId = encounter->id;
    int plural = 0;
    if (counts[0] > 1 || duplicate[0] == 1) {
        StoreInArray0x8b0(work, 0, 0);
        plural = 1;
    } else {
        StoreInArray0x8b0(work, plural, 1);
    }
    int partyCount = CopyOutRegion0x571d((char*)game, ids);
    alive = 0;
    selected = 0;
    Container_021dae60* container = GetOffsetPtr02160f08(GetActiveCombatWork());
    for (int i = 0; i < partyCount; ++i) {
        GameObject* object = game->GetCombatantByIndex(ids[i]);
        FindSlotById_021dae60(container, ids[i]);
        if (object && TestBitAt0x34((unsigned char*)party, ids[i])) {
            ++selected;
            if (!(((CombatantView*)object)->status->flags & 1)) ++alive;
        }
    }
    work->selectedCount = selected;
    work->aliveCount = alive;
    int message;
    switch (distinct) {
    case 1: message = 6; break;
    case 2:
    {
        message = 7;
        int mode = 1;
        if (counts[1] > 1 || duplicate[1] == 1) mode = 0;
        StoreInArray0x8b0(work, 1, mode);
        plural = 1;
        break;
    }
    default:
        if (encounterId == 0x36 || encounterId == 0x43 || encounterId == 0x50) message = 8;
        else { plural = 1; message = 9; }
        break;
    }
    strcpy(text, EncounterMessage(message));
    if (state->mode == 1) {
        Container_021dae60* battle = GetOffsetPtr02160f08(func_ov017_021b8468(context));
        unsigned char side = (int)party->side % 2;
        if (battle) battle->side = side;
        message = data_ov026_021de6a6[side];
        if (plural) {
            message = data_ov026_021de6a8[side];
            work->plural = 1;
        }
        strcat(text, EncounterMessage(message));
    } else if (state->mode == 2) {
        strcat(text, data_ov026_021deebd);
        int side = (int)party->side % 2;
        char* label = InitAndClearBuffer_021d8a78((void*)(int)data_ov026_021de6a2[side]);
        strcat(text, label);
        if (plural) work->plural = 1;
    }
    if (state->mode != 0) strcat(text, data_ov026_021deec7);
    else strcat(text, data_ov026_021deed8);
    func_0204500c(work, text, 0, 0xe3);
    work->field19b1 = 0;
    func_02042b98(work, 2, 0x92, 0xfc, 0x4a);
    work->field19b2 = 0;
    work->flags |= 2;
    work->active = 1;
}

