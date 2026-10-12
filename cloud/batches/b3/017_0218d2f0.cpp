#include <globaldefs.h>
#include "GameState/GameState.h"

struct PartySlots { char pad0[0xf78]; signed char ids[4]; unsigned char count; };
struct RecoveryValues { unsigned int flags; unsigned short hp; unsigned short mp; };
struct MaximumValues { char pad0[0x30]; unsigned short hp; unsigned short mp; };
struct PartyCombatant { char pad0[0x130]; RecoveryValues* values; MaximumValues* maximum; };
struct BattleContext { int field0; void* controller; };
extern BattleContext data_ov017_021d82e0;
void CopyByteField0x253To0x252(void*);
unsigned char GetByteField0x252(void*);
unsigned char GetField0x397cValue(GameState*);
struct T02053fe4;
void ClearFieldBit02053fe4(T02053fe4*);
void TryClearFlags0x130(unsigned char*, unsigned short);
extern "C" void func_ov017_021c9e00(int, int, int, int);
extern "C" void func_ov017_02191108(void*, int, int, int, int);
void SetByteField0x253(void*);
struct Obj020397cc;
void CancelPendingAction020397cc(Obj020397cc*, int);

// USA: func_ov017_0218d2f0
extern "C" ARM void func_ov017_0218d2f0(int revive, int cure, int amount, int all) {
    GameState* game = GameState::GetInstance();
    PartySlots* party = (PartySlots*)GetPtrField0x2a04(game);
    unsigned char count = party->count;
    int active = 1;
    GameObject* leader = game->GetUnknownGameObject();
    if (leader) {
        CopyByteField0x253To0x252(leader);
        if (!GetByteField0x252(leader)) active = 0;
    }
    for (unsigned char i = 0; i < count; ++i) {
        int index = party->ids[i];
        PartyCombatant* combatant = (PartyCombatant*)GetCombatantWithFlag0x100(game, index);
        if (!combatant) continue;
        if (!all && index != GetField0x397cValue(game)) continue;
        if (revive && (combatant->values->flags & 1)) {
            ClearFieldBit02053fe4((T02053fe4*)combatant);
            TryClearFlags0x130((unsigned char*)combatant, 1);
        }
        if (cure && !(combatant->values->flags & 1)) {
            if (combatant->values->flags) combatant->values->flags &= 4;
        }
        if (amount && !(combatant->values->flags & 1)) {
            int hp = combatant->values->hp + amount;
            if (combatant->maximum->hp < hp) hp = combatant->maximum->hp;
            combatant->values->hp = hp;
            int mp = combatant->values->mp + amount;
            if (combatant->maximum->mp < mp) mp = combatant->maximum->mp;
            combatant->values->mp = mp;
        }
        func_ov017_021c9e00(index, 1, 0, 1);
        func_ov017_02191108(data_ov017_021d82e0.controller, 1, 1, 1, 1);
    }
    leader = game->GetUnknownGameObject();
    if (leader) {
        if (active) SetByteField0x253(leader);
        else CancelPendingAction020397cc((Obj020397cc*)leader, 1);
    }
}
