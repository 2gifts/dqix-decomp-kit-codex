#include <globaldefs.h>
#include "GameState/GameState.h"

struct ListNode021600f8 { char pad0[0xc]; short alternateId; short id; };
struct List021600f8 {
    unsigned short action;
    char pad2[7];
    unsigned char count;
    char padA[2];
    unsigned char partyFlag : 1;
    unsigned char flags1 : 2;
    unsigned char monsterFlag : 1;
    unsigned char flags4 : 4;
};
struct EffectEntry { unsigned short id; char pad2[2]; EffectEntry* next; };
typedef unsigned long long EffectBits __attribute__((aligned(4)));
struct Effect { EffectBits flags; EffectEntry* entries; };
struct ActionDisplay {
    char pad0[0x5c5];
    unsigned char counter;
    char pad5c6[6];
    unsigned char active;
    char pad5cd[3];
    unsigned char blocked;
    unsigned char enabled;
    short actorIds[9];
    int actorCount;
    char command[0x738-0x5e8];
    unsigned char busy;
    char pad739[0xf];
    unsigned short timer;
};
struct CombatStatus { char pad0[0x28]; short relatedActor; };
void* GetActiveCombatWork();
List021600f8* GetSlotPtr02160f20(void*);
ListNode021600f8* GetNodeAtIndex021600f8(List021600f8*, int);
extern "C" short func_ov000_0215ffa0(ListNode021600f8*);
extern "C" int func_ov000_0215fd90(Effect*, unsigned int);
void ResetFields_021ed35c(char*);
extern "C" void func_ov025_021ea958(ActionDisplay*, int, int, int);
extern "C" int func_ov025_021ed380(char*, unsigned short, short, int, Effect*, short, unsigned short);
extern "C" void func_ov025_021ead5c(ActionDisplay*, int);
inline int IsPartyActor(int id) { return id >= 0 && id <= 3; }

// USA: func_ov025_021ea604
extern "C" ARM void func_ov025_021ea604(ActionDisplay* display, int index, int, Effect* effect) {
    GameState* game = GameState::GetInstance();
    List021600f8* list = GetSlotPtr02160f20(GetActiveCombatWork());
    ListNode021600f8* node = GetNodeAtIndex021600f8(list, index);
    int force = 0;
    if (display->blocked) {
        int lastSide = 1;
        int inRange = 0;
        if (node->id >= 0 && node->id <= 3) inRange = 1;
        bool party = inRange;
        for (int i = index + 1; i < list->count; i++) {
            ListNode021600f8* other = GetNodeAtIndex021600f8(list, i);
            int otherIn = 0;
            if (other->id >= 0 && other->id <= 3) otherIn = 1;
            bool otherParty = otherIn;
            if ((party && otherParty) || (!party && !otherParty)) { lastSide = 0; break; }
        }
        for (EffectEntry* entry = effect->entries; entry; entry = entry->next) {
            if (entry->id == 8) {
                display->actorIds[display->actorCount] = func_ov000_0215ffa0(node);
                display->actorCount++;
                entry->id = 0;
            }
        }
        if (lastSide) { force = 1; display->enabled = force; }
    }
    if (node) {
        int actorId = func_ov000_0215ffa0(node);
        if (!effect) return;
        if (func_ov000_0215fd90(effect, 11)) {
            GameObject* actor = game->GetCombatantByIndex(node->id);
            if (actor) actorId = ((CombatStatus*)actor->currentStats_)->relatedActor;
        }
        if (list->action == 0x399) actorId = node->alternateId;
        int reset = 0;
        if (!display->busy && display->timer < 500 && !display->active) reset = 1;
        int show = 1;
        if (IsPartyActor(actorId) && list->partyFlag && !func_ov000_0215fd90(effect, 1)) show = 0;
        else if (!IsPartyActor(actorId) && list->monsterFlag && !func_ov000_0215fd90(effect, 1)) show = 0;
        if (show && reset) ResetFields_021ed35c(display->command);
        if (show || force) {
            int update = 0;
            if (force) update = 1;
            int selection = (short)actorId;
            if (force) selection = -2;
            func_ov025_021ea958(display, IsPartyActor(actorId) ? 1 : 0, update, selection);
        }
        for (EffectEntry* entry = effect->entries; entry; entry = entry->next) {
            func_ov025_021ed380(display->command, entry->id, actorId, 1, effect, -1, 0);
        }
        if (display->active && func_ov000_0215fd90(effect, 2)) func_ov025_021ead5c(display, IsPartyActor(actorId) ? 1 : 0);
    }
    display->counter++;
}
