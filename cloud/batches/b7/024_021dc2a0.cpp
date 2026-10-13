#include <globaldefs.h>
#include "GameState/GameState.h"

struct CombatTarget : GameObject {
    char pad13c[0x17c - 0x13c];
    signed char variant;
    char pad17d[0xb];
    unsigned short currentHP;
    unsigned short currentMP;
};
GameObject* GetCombatantWithFlag0x400ByID(int, int);
struct BattleWorld {
    char pad0[0x8e01];
    unsigned char summoned;
    unsigned char actions;
};
struct Obj021600cc { short action; };
struct Node021600cc;
struct BattleAction {
    int field0;
    Obj021600cc* list;
    char pad8[8];
    BattleWorld* world;
    int count;
};
struct ActionData {
    int field0;
    unsigned int key:12;
    unsigned int upper4:20;
    char pad8[0x10];
    unsigned int lower18:5;
    unsigned int type:7;
    unsigned int upper18:20;
    char pad1c[8];
    unsigned int lower24:20;
    unsigned int fallback:10;
    unsigned int upper24:2;
};
struct Out0215fb54 { int ids[2]; };
struct Struct0215fef0 {
    char pad0[0xe];
    short id;
    char pad10[7];
    unsigned char active;
    char pad18[8];
    int next;
};
struct ActionWork {
    char pad0[0xe];
    unsigned short currentHP;
    unsigned short currentMP;
    unsigned short maximumHP;
    unsigned short maximumMP;
    char pad16[0xa];
    int next;
};
void* GetActiveCombatWork();
int FindRecordByKey0215fb54(int, short, Out0215fb54*);
void* FindNodeByShortKey02162d88(void*, short);
extern "C" int func_ov000_02161380(void*, int, int, int);
void* GetTableEntry0x8e01Bound0x88(void*);
void ResetStruct0215fef0(Struct0215fef0*);
extern "C" ActionWork* func_ov000_0215e958(BattleWorld*);
void ZeroWorkBlock0x20(void*);
void AppendToChainAndIncCount0215ffc4(void*, void*, int);
void AppendNode021600cc(Obj021600cc*, Node021600cc*);
void AddEntryAndIncrementCount0215a88c(void*, void*, int);
extern "C" void func_ov000_0215cd44(BattleWorld*, ActionWork*, GameObject*, int, int, int, int);
extern const Out0215fb54 data_ov024_021fe6d0;

// USA: func_ov024_021dc2a0
extern "C" ARM ActionWork* func_ov024_021dc2a0(BattleAction* self, int targetID, int unused, ActionData* action, int unused2, int unused3, unsigned char enabled) {
    CombatTarget* target = (CombatTarget*)GetCombatantWithFlag0x400ByID((int)self->world, targetID);
    if (!target) return 0;
    void* combat = GetActiveCombatWork();
    int targetCount = 1;
    Out0215fb54 targets = data_ov024_021fe6d0;
    targets.ids[0] = target->obj3D_.unknown_2_;
    if (action->type == 12) {
        targetCount = FindRecordByKey0215fb54((int)self->world, action->key, &targets);
        for (int i = 0; i < targetCount; ++i) {
            if (!FindNodeByShortKey02162d88(combat, targets.ids[i])) {
                targetCount = 0;
                break;
            }
        }
    }
    int variant = -1;
    if (action->key == 0xf2) variant = target->variant;
    int created = 0;
    if (enabled) {
        for (int i = 0; i < targetCount; ++i) {
            for (int attempt = 0; attempt < 1; ++attempt) {
                int id = func_ov000_02161380(combat, targets.ids[i], variant, 1);
                if (id < 0xc0) continue;
                Struct0215fef0* node = (Struct0215fef0*)GetTableEntry0x8e01Bound0x88(self->world);
                if (!node) continue;
                ResetStruct0215fef0(node);
                node->next = 0;
                node->id = id;
                node->active = 1;
                CombatTarget* summoned = (CombatTarget*)GetCombatantWithFlag0x400ByID((int)self->world, id);
                if (!summoned) continue;
                ActionWork* work = func_ov000_0215e958(self->world);
                if (!work) continue;
                ZeroWorkBlock0x20(work);
                work->next = 0;
                work->currentHP = summoned->currentHP;
                work->currentMP = summoned->currentMP;
                work->maximumHP = summoned->currentHP;
                work->maximumMP = summoned->currentMP;
                AppendToChainAndIncCount0215ffc4(node, work, 0);
                ++self->world->actions;
                AppendNode021600cc(self->list, (Node021600cc*)node);
                ++self->world->summoned;
                ++self->count;
                ++created;
            }
        }
    }
    if (created <= 0) {
        ActionWork* work = func_ov000_0215e958(self->world);
        if (!work) return 0;
        self->list->action = 0x1f5;
        AddEntryAndIncrementCount0215a88c(self->world, work, (unsigned short)action->fallback);
        GameObject* original = GetCombatantWithFlag0x400ByID((int)self->world, targetID);
        if (!original) return 0;
        func_ov000_0215cd44(self->world, work, original, 0, 0, 0, 0);
        return work;
    }
    return 0;
}
