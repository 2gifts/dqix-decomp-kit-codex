#include <globaldefs.h>
#include "GameState/GameState.h"

struct BitField0203402c;
struct Obj020397cc;
struct FlagWord02046708;
struct List0204b0e8;
struct Entry_02199684 { char field_0[0x14]; int value; char field_18[8]; };
struct ActionTail02199f08 { Entry_02199684 entries[2]; char field_40[0x38c]; int active; };
struct ActionObject02199f08 {
    Object3D object;
    short action;
    char field_ae[4];
    short timer;
    char field_b4[0xd8];
    unsigned int flags;
};
struct PartyRoster02199f08 { char field_0[0xf78]; unsigned char members[4]; unsigned char count; };
int IsField0Null(void**);
void CancelActionIfHeadIdMatches_02199e98(void*);
int GetIntField0x260(void*);
int CheckField0xc4Low15BitsNonZero(BitField0203402c*);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);
int GetByteField0x252(void*);
int CheckSubstructByte0x7cPositive(signed char*);
void CancelPendingAction020397cc(Obj020397cc*, int);
unsigned int* GetDataPtr02114e04_020d6c00();
void OrBitsIntoField0(unsigned int*, unsigned int);
extern "C" void func_ov017_021a5bac(void*);
void InitFieldArenaListsAndCounters_021996fc(unsigned char*);
int TestFlagInSecondWord(unsigned short*, unsigned int);
int TestFlags02046708(FlagWord02046708*, unsigned int);
extern "C" void func_ov017_021999fc(void*);
int TestFlagMask(unsigned short*, int);
void FillEntriesEncoded_02199684(Entry_02199684*, int, int);
extern "C" void func_0204bc74(void*, int, int, int, int, int, int);
void FlushAndDispatchList0204b0e8(List0204b0e8*, void*);
void ClearAndEncodeEntries_021996bc(Entry_02199684*, int);
extern unsigned short data_02114e30[];
#define BG1CONTROL (*(volatile unsigned short*)0x0400000a)
#define BG2CONTROL (*(volatile unsigned short*)0x0400000c)

// USA: func_ov017_02199f08
extern "C" ARM void func_ov017_02199f08(GameResources* arena) {
    if (!IsField0Null((void**)arena->unknown_ptr_array_36fc[0])) {
        CancelActionIfHeadIdMatches_02199e98(arena);
        return;
    }
    GameState* state = GameState::GetInstance();
    state->GetProtagonist();
    ActionObject02199f08* actor = (ActionObject02199f08*)state->GetUnknownGameObject();
    PartyRoster02199f08* party = (PartyRoster02199f08*)GetPtrField0x2a04(state);
    int action = GetIntField0x260(actor);
    if (action == -1 && !CheckField0xc4Low15BitsNonZero((BitField0203402c*)actor) && !((ActionTail02199f08*)arena->unknown_3cb4)->active) {
        if (!TestFlag0SetAndFlag1Clear(data_02114e30, 2)) return;
        if (!GetByteField0x252(actor)) return;
        if (actor->flags & 1) return;
        if (CheckSubstructByte0x7cPositive((signed char*)actor) || actor->action) return;
        CancelPendingAction020397cc((Obj020397cc*)actor, 1);
        actor->timer = 0;
        OrBitsIntoField0(GetDataPtr02114e04_020d6c00(), 0x400);
        func_ov017_021a5bac(arena);
        union BackgroundControl { unsigned short raw; short value; } first, second;
        first.raw = BG1CONTROL;
        *(short*)arena->unknown_ptr_array_3ca4[1] = first.value;
        second.raw = BG2CONTROL;
        *(short*)arena->unknown_ptr_array_3ca4[2] = second.value;
        InitFieldArenaListsAndCounters_021996fc((unsigned char*)arena);
        ((ActionTail02199f08*)arena->unknown_3cb4)->active = 1;
        return;
    }
    if (TestFlagInSecondWord(data_02114e30, 2) || actor->action || CheckField0xc4Low15BitsNonZero((BitField0203402c*)actor)) {
        CancelActionIfHeadIdMatches_02199e98(arena);
        return;
    }
    if (action != -1) {
        for (int i = 0; i < party->count; ++i) {
            ((ActionObject02199f08*)state->GetPartyMemberByIndex(party->members[i]))->action = 0;
        }
    }
    if (TestFlags02046708((FlagWord02046708*)GetDataPtr02114e04_020d6c00(), 4)) {
        CancelActionIfHeadIdMatches_02199e98(arena);
        return;
    }
    func_ov017_021999fc(arena);
    if (TestFlagMask(data_02114e30, 2) && action != -1) {
        FillEntriesEncoded_02199684(((ActionTail02199f08*)arena->unknown_3cb4)->entries, 2, 0x800);
        Entry_02199684* entry;
        for (int i = 0; i < 2; ++i) {
            entry = &((ActionTail02199f08*)arena->unknown_3cb4)->entries[i];
            func_0204bc74(entry, 0, 0, 0, 0x20, 0x19, 0);
            FlushAndDispatchList0204b0e8((List0204b0e8*)entry, 0);
        }
        ClearAndEncodeEntries_021996bc(((ActionTail02199f08*)arena->unknown_3cb4)->entries, 2);
    }
}
