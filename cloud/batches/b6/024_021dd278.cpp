#include <globaldefs.h>
#include "Combat/Overlay_0/GetCombatantByID.h"

struct S_10088;
int IsFlag10088Set(S_10088*);
int SelectByIndexRange0to3_021da644(int, int, int);
void ResetFields14And18KeepFlag0x4(unsigned char*, unsigned short);
void AddEntryAndIncrementCount0215a88c(void*, void*, int);
extern "C" void* func_ov000_0215e958(void*);
extern "C" void func_ov000_02159eac(void*, unsigned long long*, int);
extern "C" void func_ov000_0215cd44(void*, void*, GameObject*, int, unsigned long long, int);

struct BattleFlags021dd278 {
    char field_0[0x1c];
    unsigned char field_1c : 7;
    unsigned char alternate : 1;
};
struct BattleAction021dd278 {
    char field_0[0xc];
    BattleFlags021dd278* flags;
    void* battle;
    int successes;
};
struct PackedCodes021dd278 {
    unsigned int first : 10;
    unsigned int second : 10;
    unsigned int third : 10;
    unsigned int flags : 2;
};
struct ActionData021dd278 {
    int field_0;
    unsigned int id : 12;
    unsigned int minimum : 10;
    unsigned int maximum : 10;
    char field_8[0x18];
    PackedCodes021dd278 success;
    PackedCodes021dd278 failure;
};
struct RevivalState021dd278 {
    char field_0[0x3a];
    unsigned char revived;
    unsigned char active : 1;
    unsigned char flags : 7;
};
static inline int IsPartyMember(int id) { return id >= 0 && id <= 3; }

// USA: func_ov024_021dd278
extern "C" ARM void* func_ov024_021dd278(BattleAction021dd278* action, int sourceId, int targetId, ActionData021dd278* data, int unused, int unused2, unsigned char allowed) {
    GameObject* source = GetCombatantByID((int)action->battle, sourceId);
    if (!source) return 0;
    GameObject* target = GetCombatantByID((int)action->battle, targetId);
    if (!target) return 0;
    int dead = 0;
    if (IsFlag10088Set((S_10088*)target)) dead = 1;
    unsigned short skill = data->id;
    unsigned long long event = 0;
    int code;
    if (allowed && dead) {
        float fraction = 1.0f;
        switch (skill) {
        case 39:
            fraction = 0.5f;
            break;
        case 38:
        case 84:
            if (IsPartyMember(sourceId)) {
                unsigned short power = source->currentStats_->primaryStats.magicalMending;
                if (power <= data->minimum) fraction = 0.25f;
                else if (power >= data->maximum) fraction = 0.5f;
                else {
                    fraction = 25.0f / (float)(data->maximum - data->minimum);
                    fraction = (25.0f + (int)(fraction * (float)(power - data->minimum))) / 100.0f;
                }
            } else fraction = 0.5f;
            break;
        }
        ModifiableCombatStats* stats = target->currentStats_;
        unsigned short restored = (int)(fraction * (float)(unsigned int)stats->primaryStats.maxHP);
        ResetFields14And18KeepFlag0x4((unsigned char*)stats, restored);
        func_ov000_02159eac(action->battle, &event, 3);
        ((RevivalState021dd278*)target->currentStats_)->active = 1;
        if (IsPartyMember(targetId)) {
            GameObject* member = GetCombatantWithFlag0x100(GameState::GetInstance(), targetId);
            if (member) ((RevivalState021dd278*)member->currentStats_)->revived = 1;
        }
        code = SelectByIndexRange0to3_021da644(targetId, data->success.third, data->failure.first);
        ++action->successes;
    } else if (dead) {
        code = 0x21;
    } else {
        code = SelectByIndexRange0to3_021da644(targetId, data->failure.second, data->failure.third);
    }
    void* entry = func_ov000_0215e958(action->battle);
    if (!entry) return 0;
    AddEntryAndIncrementCount0215a88c(action->battle, entry, code);
    func_ov000_0215cd44(action->battle, entry, target, 0, event, action->flags->alternate != 0);
    return entry;
}
