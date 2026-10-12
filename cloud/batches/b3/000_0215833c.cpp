#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Util/Random.h"

struct RecoveryStats {
    unsigned int pad0[0xf];
    unsigned char lowerFlags:6;
    unsigned char blocked:1;
    unsigned char upperFlags:1;
    char pad3d[0x42];
    unsigned char sleepTurns;
    unsigned char paralysisTurns;
    unsigned char confusionTurns;
};
struct RecoveryResult {
    unsigned short message;
    char pad2[7];
    unsigned char state;
    char padA;
    unsigned char blocked:1;
    unsigned char remaining:7;
};
int IsFlag0x14Bit0x8Set(GameObject*);
int CheckFlag0x14Bit0x10Set(unsigned char*);
int IsFlag0x14Bit0x20Set(GameObject*);
void ClearFlag0x14Bit0x8AndBytes(void*);
void ClearBattleFlag0x14Bit4(void*);
void ClearFlag0x20AndBytes(void*);
extern const float data_ov000_02182ad4[];
extern const float data_ov000_02182bd4[];

// USA: func_ov000_0215833c
extern "C" ARM int func_ov000_0215833c(Random* random, int index, RecoveryResult* result) {
    float chance;
    RecoveryStats* stats;
    GameObject* combatant = GameState::GetInstance()->GetCombatantByIndex(index);
    const volatile GameObject* combatant_v = combatant;
    if (!combatant) return 0;
    if (((RecoveryStats*)combatant->currentStats_)->blocked) result->blocked = 1;
    chance = (float)NextRandomMax(random, 100) / 100.0f;
    stats = (RecoveryStats*)(unsigned int)combatant_v->currentStats_;
    if (IsFlag0x14Bit0x8Set(combatant)) {
        if (stats->sleepTurns) {
            if (data_ov000_02182ad4[--stats->sleepTurns] > chance) {
                stats->sleepTurns = 0;
                ClearFlag0x14Bit0x8AndBytes(combatant_v->currentStats_);
                result->message = 0x384;
                result->state = 0;
                return 1;
            }
        }
    } else if (CheckFlag0x14Bit0x10Set((unsigned char*)combatant_v->currentStats_)) {
        if (stats->paralysisTurns) {
            if (data_ov000_02182bd4[--stats->paralysisTurns] > chance) {
                stats->paralysisTurns = 0;
                ClearBattleFlag0x14Bit4(combatant_v->currentStats_);
                result->message = 0x385;
                result->state = 0;
                return 1;
            }
        }
    } else if (IsFlag0x14Bit0x20Set(combatant) && stats->confusionTurns) {
        if (data_ov000_02182ad4[--stats->confusionTurns] > chance) {
            stats->confusionTurns = 0;
            ClearFlag0x20AndBytes(combatant_v->currentStats_);
            result->message = 0x3aa;
            result->state = 0;
            return 1;
        }
    }
    return 0;
}
