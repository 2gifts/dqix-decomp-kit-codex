#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Grotto/Main/ActiveGrottoClass.h"

struct Battle0208a03c;
struct Context0208a03c {
    struct Battle0208a03c* battle;
};

struct MoveSet0208a03c {
    char pad0[0x10];
    unsigned int pad10a : 20;
    unsigned int usableMask : 6;
    unsigned int pad10b : 6;
    char pad14[4];
    unsigned short moves[6];
};

struct Limits0208a03c {
    char pad0[2];
    unsigned short count;
    char pad4[0x10];
    unsigned int blocked;
    char pad18[0x24];
    unsigned char lo : 3;
    unsigned char flagA : 1;
    unsigned char flagB : 1;
    unsigned char flagC : 1;
};

struct Combatant0208a03c {
    char pad0[2];
    short kindId;
    char pad4[0x134];
    struct Limits0208a03c* limits;
    char pad13c[0xc];
    struct MoveSet0208a03c* moveSet;
    char pad14c[0x30];
    unsigned char slot;
};

struct Move0208a03c {
    char pad0[8];
    unsigned int minCount : 8;
    unsigned int pad8 : 24;
    char padc[4];
    unsigned int flags;
    char pad14[4];
    unsigned int pad18a : 5;
    unsigned int effect : 7;
    unsigned int pad18b : 20;
};

struct Party0208a03c {
    char pad[0x25];
    unsigned char f25;
};

struct Battle0208a03c {
    char pad0[0x8e18];
    struct Party0208a03c* party;
    char pad8e1c[4];
    int f8e20;
    char pad8e24[0x25];
    unsigned char mode;
};

struct Combatant0208a03c* GetCombatantWithFlag0x400(GameState*, int);
void* GetData02108e10(void);
int IsGlobalU16InRange(GameState*);
extern "C" char* func_02012fe4(void);
extern "C" char* _ZN17ActiveGrottoClass15GetDetailedDataEv(void* grotto);
extern "C" unsigned short _ZNK23DetailedTreasureMapData17LegacyBossMapData26MaybeGetCurrentAlternateIDEv(void* legacy);
extern "C" int _ZN23DetailedTreasureMapData17LegacyBossMapData17CanUseLevelUpMoveEt(void* legacy, int moveId);
extern "C" struct Move0208a03c* _Z24SearchBothTables02079e2cPci(char* table, int key);
int GetCombatantField0x148Bits3To4(struct Battle0208a03c* unused, int id);
void ClearFirstField_021f66c0(int* p);
extern "C" int func_ov024_021f66cc(int* obj, struct Battle0208a03c* battle, int moveId, int id, int* out, int arg);

// USA: func_0208a03c
extern "C" ARM int func_0208a03c(Context0208a03c* ctx, unsigned char index, int id, int* out, int arg) {
    GameState* gs = GameState::GetInstance();
    struct Combatant0208a03c* c = GetCombatantWithFlag0x400(gs, id);
    char* table = (char*)GetData02108e10();
    struct MoveSet0208a03c* set = c->moveSet;
    if (ctx->battle->mode == 2 && ctx->battle->f8e20 == 0 && c->moveSet->moves[index] == 0xe1) {
        return 0;
    }
    unsigned int usable = set->usableMask;
    unsigned char* used = (unsigned char*)ctx->battle + 0x81bf + (short)c->slot * 0x18;
    if (usable & (1 << index)) {
        if (*used & (1 << index)) {
            return 0;
        }
    }
    char* zone = func_02012fe4();
    if (IsGlobalU16InRange(gs) && ctx->battle->party->f25 != 0) {
        char* detail = _ZN17ActiveGrottoClass15GetDetailedDataEv(zone + 0x23ec);
        short kindId = c->kindId;
        unsigned short alt = _ZNK23DetailedTreasureMapData17LegacyBossMapData26MaybeGetCurrentAlternateIDEv(detail + 0x4c);
        if (alt == kindId) {
            if (!_ZN23DetailedTreasureMapData17LegacyBossMapData17CanUseLevelUpMoveEt(detail + 0x4c, set->moves[index])) {
                return 0;
            }
        }
    }
    struct Move0208a03c* move = _Z24SearchBothTables02079e2cPci(table, (short)set->moves[index]);
    int passed = 0;
    int kind = GetCombatantField0x148Bits3To4(ctx->battle, (short)id);
    int applies;
    unsigned int flags = move->flags;
    if (flags & 1) {
        applies = 0;
        if (kind == 2) {
            applies = 1;
        } else if (kind == 1) {
            if (c->limits->flagA) {
                applies = 1;
            }
        }
        if (applies) {
            struct Limits0208a03c* limits = c->limits;
            if (limits->blocked & 0x100) {
                return 0;
            }
            unsigned int count = limits->count;
            if (count == 0) {
                if (move->minCount != 0) {
                    return 0;
                }
            }
            if (move->minCount < 0xff && count < move->minCount) {
                return 0;
            }
        }
        passed = 1;
    }
    if (flags & 2) {
        int applies = 0;
        if (kind == 2) {
            applies = 1;
        } else if (kind == 1) {
            if (c->limits->flagB) {
                applies = 1;
            }
        }
        if (applies) {
            struct Limits0208a03c* limits = c->limits;
            unsigned int count = limits->count;
            if (count == 0) {
                if (move->minCount != 0) {
                    return 0;
                }
            }
            if (move->minCount < 0xff && count < move->minCount) {
                return 0;
            }
        }
        passed = 1;
    }
    if (!passed) {
        applies = 0;
        if (kind == 2) {
            applies = 1;
        } else if (kind == 1) {
            if (c->limits->flagC) {
                applies = 1;
            }
        }
        if (applies) {
            struct Limits0208a03c* limits = c->limits;
            unsigned int count = limits->count;
            if (count == 0) {
                if (move->minCount != 0) {
                    return 0;
                }
            }
            if (move->minCount < 0xff && count < move->minCount) {
                return 0;
            }
        }
    }
    int handler;
    ClearFirstField_021f66c0(&handler);
    if (!func_ov024_021f66cc(&handler, ctx->battle, set->moves[index], id, out, arg)) {
        return 0;
    }
    if (move->effect != 0x22) {
        *used |= 1 << index;
    } else {
        *used = 0;
    }
    return 1;
}
