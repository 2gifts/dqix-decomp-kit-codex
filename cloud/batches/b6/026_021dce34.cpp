#include <globaldefs.h>
#include <GameState/GameState.h>
#include <std_library_functions.h>

struct BattleWork;
struct BattleView;
struct Container_021dae60;
struct Combatant020A36A8;
struct S_10088;
struct FlagObj_021dd3c4;
struct Obj02039d84;
struct Flags02033fdc;
struct Bytes02033b88;
struct BattleState { char pad0[0x8e20]; int turns; };
struct PartyState {
    unsigned short field0, field2;
    char pad4[4]; unsigned short netId; char pada[2]; int fieldc;
    char pad10[0x1a]; signed char memberIndex; char pad2b[9]; unsigned char memberMask; unsigned char mode;
};
struct SlotState {
    char pad0[0x10]; unsigned char actions[8]; signed char actionIndex;
    char pad19[0xb]; unsigned char flags; char pad25[3]; unsigned int memberFlags;
};
struct MemberFlags { unsigned int flags; };
struct MemberView {
    Object3D object;
    char padac[0x12]; unsigned char state; char padbf[2];
    unsigned char flagsLow : 2; unsigned char flagsHigh : 6;
    unsigned char flagsC2; char padc3[0x6d];
    MemberFlags* memberFlags;
    BaseCombatStats* baseStats;
    ModifiableCombatStats* currentStats;
    char pad13c[0x50]; unsigned int flags18c;
};
struct MemberContext { unsigned short objectFlags; char pad2[6]; void* field8; };
struct SavedMemberData { char data[0x70]; };
struct Field0x19cData { int values[3]; };
struct SavedPosition { Vector3fix position; short angle; short pad; };
struct Src021cbc48 {
    unsigned short netId, field2;
    int eventId;
    unsigned char memberMask;
    signed char memberIndex;
    unsigned char activeParty, activeEnemies, mode;
    short fieldE;
};
int MatchesActiveIndex020a36a8(Combatant020A36A8*);
int GetCombatWorkFlags0x55f4(void*, int);
void ClearCombatWorkFlags0x55f4(void*, int);
void SetBitInField0x34(unsigned char*, int);
void* FindSlotById_021dae60(Container_021dae60*, int);
void SetAndClearField0x19c(char*, Field0x19cData*);
void CopyCombatantStats0208936c(char*, char*, char*);
void ResetObjectState02039d84(Obj02039d84*);
void ClearFlag0x4At0xe0(Flags02033fdc*);
void SetByte0xbeShiftPrev(Bytes02033b88*, int);
void ClearField0xc4KeepBit0x8000(unsigned char*);
void ClearFlag0x1ceBit0x4(unsigned char*);
void AddEntry02168644(char*, int);
void SetFlag0x1ceBit0x1(unsigned char*);
void RunFlaglearAndSetMode02167e6c_02167e6c(unsigned char*);
int IsFlag10088Set(S_10088*);
int IsFlagBit8192Set_021dd3c4(FlagObj_021dd3c4*);
int TestBitAt0x34(unsigned char*, unsigned int);
void EnqueueEventTag100_021cbc48(Src021cbc48*);
void DispatchAllEventStages_021cdf80(char*, unsigned char);
int ClassifyField0x81fe(char*);
void Process02181364(void*);
void ClearFlag_021816e8_021816e8(void*, int);
void SetFlag_02181694_02181694(void*, int);
void ClearByteField_0217f5d0_0217f5d0(void*);
extern "C" {
int* func_0202ae18();
int func_0202c540(int*);
int func_ov000_0215fad4(BattleState*);
int func_ov000_0215e9fc(BattleState*, short*, int, int);
void func_ov000_02174738(Container_021dae60*, int);
void func_02049c88(MemberView*, SavedMemberData*);
void func_ov026_021dc8fc(BattleWork*, BattleState*, PartyState*, Container_021dae60*, void*, void*, int);
void func_020531f0(MemberView*);
void func_ov000_02167fb0(BattleWork*, MemberView*);
void func_ov017_0219c774(void*, int, int);
void func_ov017_021917f0(int, int);
void func_ov000_021675a0(BattleWork*);
void func_ov026_021daec8(BattleState*, BattleView*, int);
void func_ov000_021676a8(BattleWork*);
void func_ov000_02174b14(Container_021dae60*);
void func_ov000_0217a8f4(Container_021dae60*);
void func_ov017_021c847c();
void func_ov000_0217fcc4(Container_021dae60*, int);
}

// USA: func_ov026_021dce34
extern "C" ARM void func_ov026_021dce34(int eventId, BattleWork* self, BattleState* battle, PartyState* party, Container_021dae60* slots, MemberContext* context, SavedMemberData* saved, Field0x19cData* fields, void* data1, void* data2, SavedPosition* positions, BattleView* view) {
    GameState* game = GameState::GetInstance();
    func_ov017_0218b5b0();
    int* link = func_0202ae18();
    if (!MatchesActiveIndex020a36a8((Combatant020A36A8*)party) && !GetCombatWorkFlags0x55f4(self, 0x80000000)) return;
    ClearCombatWorkFlags0x55f4(self, 0x80000000);
    int changed = 0;
    int memberId = func_ov000_0215fad4(battle);
    short members[4];
    func_ov000_0215e9fc(battle, members, 4, 0);
    while (memberId >= 0) {
        SetBitInField0x34((unsigned char*)party, (unsigned char)memberId);
        func_ov000_02174738(slots, memberId);
        SlotState* slot = (SlotState*)FindSlotById_021dae60(slots, memberId);
        if (slot) {
            MemberView* member = (MemberView*)game->GetPartyMemberByIndex(memberId);
            if (member) slot->memberFlags = member->memberFlags->flags;
        }
        if (eventId >= 0) {
            SlotState* slot = (SlotState*)FindSlotById_021dae60(slots, memberId);
            if (slot) { slot->actions[slot->actionIndex] = 100; slot->flags &= ~1; }
        }
        unsigned short objectFlags = context->objectFlags;
        MemberView* member = (MemberView*)game->GetPartyMemberByIndex(memberId);
        if (member) {
            member->object.MakeHidden();
            member->object.SetField06(objectFlags);
            memset(&saved[memberId], 0, sizeof(SavedMemberData));
            func_02049c88(member, &saved[memberId]);
            SetAndClearField0x19c((char*)member, &fields[memberId]);
            CopyCombatantStats0208936c((char*)member->currentStats, (char*)member->memberFlags, (char*)member->baseStats);
            func_ov026_021dc8fc(self, battle, party, slots, data1, data2, memberId);
            func_020531f0(member);
            ResetObjectState02039d84((Obj02039d84*)member);
            member->flagsLow |= 3;
            member->flagsC2 |= 0x20;
            ClearFlag0x4At0xe0((Flags02033fdc*)member);
            SetByte0xbeShiftPrev((Bytes02033b88*)member, 0);
            ClearField0xc4KeepBit0x8000((unsigned char*)member);
            member->object.SetInheritedAlpha(31);
            ClearFlag0x1ceBit0x4((unsigned char*)member);
            if (!(member->memberFlags->flags & 1) && (member->flags18c & 1)) member->flags18c &= ~1;
            if ((member->memberFlags->flags & 1) && !(member->flags18c & 1)) member->flags18c |= 1;
            positions[memberId].position = member->object.position_;
            positions[memberId].angle = member->object.rotation_.y;
            changed = 1;
            AddEntry02168644((char*)self, memberId);
            SetFlag0x1ceBit0x1((unsigned char*)member);
            func_ov000_02167fb0(self, member);
            if (member->object.unknown_4_ == 0 && func_0202c540(link)) {
                func_ov017_0219c774(context->field8, 0, 0);
            }
        }
        func_ov017_021917f0(memberId, 1);
        memberId = func_ov000_0215fad4(battle);
    }
    if (changed) {
        ClearCombatWorkFlags0x55f4(self, 0x80000000);
        func_ov000_021675a0(self);
        RunFlaglearAndSetMode02167e6c_02167e6c((unsigned char*)self);
        func_ov026_021daec8(battle, view, 0);
        if (!battle->turns) func_ov000_021676a8(self);
        func_ov000_02174b14(slots);
        func_ov000_0217a8f4(slots);
        if (MatchesActiveIndex020a36a8((Combatant020A36A8*)party)) {
            unsigned char activeParty = 0;
            unsigned char activeEnemies = 0;
            short currentMembers[4];
            int count = func_ov000_0215e9fc(battle, currentMembers, 4, 0);
            for (int index = 0; index < count; ++index) {
                MemberView* member = (MemberView*)game->GetCombatantByIndex(currentMembers[index]);
                if (member && member->state != 4 && member->state != 6) activeParty |= 1 << index;
            }
            for (int index = 0; index < 8; ++index) {
                GameObject* member = game->GetCombatantByIndex(index + 0xc0);
                if (member && member->obj3D_.IsVisible() && !IsFlag10088Set((S_10088*)member) && !IsFlagBit8192Set_021dd3c4((FlagObj_021dd3c4*)member)) activeEnemies |= 1 << index;
            }
            Src021cbc48 event;
            event.netId = party->netId;
            event.eventId = eventId;
            event.activeEnemies = activeEnemies;
            event.activeParty = activeParty;
            event.mode = party->mode;
            event.fieldE = party->fieldc;
            event.memberMask = 0;
            for (int index = 0; index < 4; ++index) {
                if (TestBitAt0x34((unsigned char*)party, (unsigned char)index)) event.memberMask |= 1 << index;
                else event.memberMask &= ~(1 << index);
            }
            event.memberIndex = party->memberIndex;
            event.field2 = party->field2;
            EnqueueEventTag100_021cbc48(&event);
            DispatchAllEventStages_021cdf80((char*)battle, 1);
            func_ov017_021c847c();
            if (party->mode == 2) func_ov000_0217fcc4(slots, -1);
        }
    }
    if (ClassifyField0x81fe((char*)battle)) {
        if (func_0202c540(func_0202ae18())) {
            Process02181364(slots);
            func_ov000_0217fcc4(slots, -1);
            ClearFlag_021816e8_021816e8(slots, -1);
            SetFlag_02181694_02181694(slots, 0);
        } else {
            for (int index = 1; index < 4; ++index) {
                SlotState* slot = (SlotState*)FindSlotById_021dae60(slots, index);
                if (slot) { ClearByteField_0217f5d0_0217f5d0(slot); slot->actions[slot->actionIndex] = 100; }
            }
        }
    }
}
