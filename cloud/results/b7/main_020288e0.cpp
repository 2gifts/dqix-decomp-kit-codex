#include <globaldefs.h>
#include "GameState/GameState.h"

struct BigRecord020289c4 {
    unsigned short field0;
    unsigned short flagsX : 2;
    unsigned short flagB : 1;
    unsigned short flagC : 1;
    unsigned short flagD : 12;
    unsigned short field4;
    unsigned short field6;
    unsigned int field8;
    unsigned char fieldC;
    signed char fieldD;
    char fields[0x318 - 0xe];
};
struct Combatant020288e0 {
    GameObject object;
    char field_0x13c[0x15c - 0x13c];
    int identifier;
};
GameObject* GetCombatantWithFlag0x200(GameState*, int);
int GetField0x158(void*);
void ClearCombatantSlot(GameState*, int);
void InitializeSubObjectsAndFields020289c4(BigRecord020289c4*);

// USA: func_020288e0
extern "C" ARM void func_020288e0(BigRecord020289c4* records) {
    int slot;
    int base;
    GameState* state = GameState::GetInstance();
    int i = 0;
    while (i < 4) {
        unsigned short identifier;
        int found;
        int member;
        Combatant020288e0* combatant;
        BigRecord020289c4* record;
        record = &records[i];
        identifier = record->field0;
        if (record->flagB) {
            found = 0;
            member = 0;
            while (member < 4) {
                combatant = (Combatant020288e0*)GetCombatantWithFlag0x200(state, member);
                if (combatant) {
                    if (GetField0x158(combatant) == 0) {
                        if (identifier != combatant->object.obj3D_.GetField06()) goto nextMember;
                    } else {
                        if (identifier != combatant->identifier) goto nextMember;
                    }
                    found = 1;
                    break;
                }
            nextMember:
                member++;
            }
            if (!found) {
                base = i * 12;
                slot = 0;
                base += 0x70;
                while (slot < 12) {
                    ClearCombatantSlot(state, base + slot);
                    slot++;
                }
                InitializeSubObjectsAndFields020289c4(record);
            }
        }
        i++;
    }
}
