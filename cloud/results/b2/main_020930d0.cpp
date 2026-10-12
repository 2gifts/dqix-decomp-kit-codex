#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/GameResources.h"

struct ListHead02046b60;

extern "C" void func_02092d60(void* s, int b);
extern "C" int func_020457e0(char* g);
extern "C" unsigned char* func_ov017_021b8478(void* p);
extern "C" void func_02094030(int* obj, short a, short b, int c);
extern "C" char* _Z26GetGlobalField0x1c020421a0v(void);
extern "C" void _Z24ReinitController02043204Pc(char* obj);
void* GetPtrField0x2a04(GameState* gs);
int* GetGlobal02109030(void);
unsigned int CountMatchingTableEntries02092cd0(int s, int kind);
int FindMatchingTableIndex02092d18(int s, int kind);
int ListContainsId(struct ListHead02046b60* list, int id);
int CountSetBitsAt0x34(unsigned char* p);

struct BattlePhaseState020930d0 {
    char unk0[0x30];
    short field30;
    short field32;
    unsigned char field34;
    unsigned char field35;
    char unk36[0x39 - 0x36];
    signed char field39;
    char unk3a;
    unsigned char field3b;
};

struct CombatantIndexList020930d0 {
    char unk0[0xf7c];
    unsigned char count;
};

struct GameStateFlags020930d0 {
    char unk0[0x7f76];
    unsigned char slots[4];
    unsigned char flags;
};

struct GlobalBlock020930d0 {
    char unk0[0x998];
    int field998;
    char unk99c[0x9a0 - 0x99c];
    int field9a0;
};

// USA: func_020930d0
extern "C" ARM void func_020930d0(struct BattlePhaseState020930d0* s) {
    GameResources* res;
    GameState* gs;
    GlobalBlock020930d0* gf;
    int* g;
    unsigned int tableCount;
    unsigned char present;
    int i;
    struct CombatantIndexList020930d0* party;
    struct ListHead02046b60* list;
    unsigned char state;

    gs = GameState::GetInstance();
    gf = (GlobalBlock020930d0*)_Z26GetGlobalField0x1c020421a0v();
    res = func_ov017_0218b5b0();
    list = (struct ListHead02046b60*)res->unknown_ptr_array_36fc[0];
    party = (struct CombatantIndexList020930d0*)GetPtrField0x2a04(gs);
    g = GetGlobal02109030();
    state = s->field35;

    if (state == 0) {
        s->field30 = 0;
        s->field35++;
        return;
    }

    if (state == 1) {
        tableCount = CountMatchingTableEntries02092cd0((int)s, 1);
        present = 0;
        for (i = 0; i < 4; i++) {
            if (gs->GetPartyMemberByIndex(i)) present++;
        }
        unsigned char need = party->count;
        unsigned char missing = present - need;
        unsigned char total = need + missing;
        int ready = 0;
        if (missing == 0) {
            ready = 1;
        } else if (tableCount == 0) {
            tableCount = 0;
            for (int i = 0; i < 4; i++) {
                if (((struct GameStateFlags020930d0*)gs)->flags & (1 << i)) tableCount = (tableCount + 1) & 0xff;
            }
            if (tableCount >= missing) ready = 1;
        }
        if (ready) {
            if (s->field32 != 0) return;
            _Z24ReinitController02043204Pc((char*)gf);
            int next;
            int c0 = CountMatchingTableEntries02092cd0((int)s, 0);
            int c5 = CountMatchingTableEntries02092cd0((int)s, 5);
            if (c0 == 4 && missing == 0) {
                next = 8;
            } else {
                next = 6;
                if (ListContainsId(list, 10)) {
                    if (total == CountSetBitsAt0x34(func_ov017_021b8478(res->unknown_ptr_3718)) && c0 + c5 == 4) s->field3b = 1;
                }
            }
            s->field30 = next;
            s->field35 = 3;
        } else {
            if (gf->field998 != 0) return;
            s->field30 = 5;
            s->field35++;
        }
        return;
    }

    if (state == 2) {
        if (gf->field9a0 != 0) return;
        int r = func_020457e0((char*)gf);
        if (r == 0) {
            s->field35 = 3;
            func_02092d60(s, 0);
            return;
        }
        if (r == 1) {
            s->field35 = 1;
            s->field32 = 0;
        }
        return;
    }

    if (state == 3) {
        if (s->field39 < 10) {
            s->field39 += gs->GetTickCount();
            return;
        }
        if (s->field3b != 0) {
            s->field3b = 0;
            func_02094030(g, 0x4e24, -1, 0);
        }
        if (CountMatchingTableEntries02092cd0((int)s, 2) > 1) {
            func_02094030(g, 0x4e22, -1, FindMatchingTableIndex02092d18((int)s, 2));
        } else if (CountMatchingTableEntries02092cd0((int)s, 2) == 1) {
            func_02094030(g, 0x4e21, -1, FindMatchingTableIndex02092d18((int)s, 2));
        }
        for (unsigned char i = 0; i < 4; i++) {
            if (((struct GameStateFlags020930d0*)gs)->slots[i] == 4) {
                func_02094030(g, 0x2711, -1, (signed char)i);
            }
        }
        s->field35++;
        return;
    }

    if (state == 4) {
        if (gf->field998 == 0) {
            s->field34 = 4;
            s->field35 = 0;
        }
    }
}
