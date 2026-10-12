#include <globaldefs.h>
#include "std_library_functions.h"

struct GameObject;
struct ArrayContainsByteStruct;
struct Outer_02054000;
struct Field150Holder02052df8;
struct S_a0840;
struct S_a02d0;
struct S_a0644;

struct Buffs021554f4 {
    int attack : 3;
    int defense : 3;
    int agility : 3;
    int charm : 3;
    int rest : 20;
};

struct Stats021554f4 {
    unsigned short currHP;
    char pad2[0x14 - 0x2];
    unsigned int status;
    unsigned int flags;
    char pad1c[0x21 - 0x1c];
    unsigned char rank;
    unsigned short colorA : 2;
    unsigned short colorB : 4;
    unsigned short colorC : 3;
    unsigned short colorD : 3;
    unsigned short colorRest : 4;
    unsigned char style;
    char pad25[0x2e - 0x25];
    short targetId;
    char pad30[0x58 - 0x30];
    struct Buffs021554f4 buffs;
};

struct Info021554f4 {
    char pad0[8];
    short id;
};

struct Combatant021554f4 {
    char pad0[0x138];
    struct Stats021554f4* stats;
    char pad13c[0x144 - 0x13c];
    struct Info021554f4* info;
    char pad148[0x181 - 0x148];
    unsigned char guarded;
    unsigned char countable;
};

struct ActiveSub021554f4 {
    char pad0[8];
    unsigned int lo : 4;
    unsigned int tier : 5;
    unsigned int rest : 23;
};

struct Source021554f4 {
    char pad0[4];
    unsigned int id : 12;
    unsigned int rest : 20;
};

struct Entry021554f4 {
    unsigned int status;
    unsigned int flags;
    unsigned int killerStatus;
    unsigned int killerFlags;
    short infoId;
    short killer;
    unsigned short sourceId;
    unsigned short turn;
    unsigned short colorA : 2;
    unsigned short colorB : 4;
    unsigned short colorC : 3;
    unsigned short colorD : 3;
    unsigned short colorRest : 4;
    unsigned short killerColorA : 2;
    unsigned short killerColorB : 4;
    unsigned short killerColorC : 3;
    unsigned short killerColorD : 3;
    unsigned short killerColorRest : 4;
    unsigned char rank : 4;
    unsigned char style : 4;
    unsigned char killerRank : 4;
    unsigned char killerStyle : 4;
    unsigned char kind;
    unsigned char extra;
    float ratio;
    unsigned char tier;
    unsigned char areaA;
    unsigned char areaB;
    char pad27;
    struct Buffs021554f4 buffs;
    struct Buffs021554f4 killerBuffs;
    short skills[8];
};

struct BattleInfo021554f4 {
    char pad0[0x24];
    unsigned char areaA;
    unsigned char areaB;
};

struct Root021554f4 {
    char pad0[0x5c60];
    struct Entry021554f4 entries[16];
    char pad6060[0x8e04 - 0x6060];
    short lastId;
    signed char lastIndex;
    unsigned char entryCount;
    char pad8e08[0x8e18 - 0x8e08];
    struct BattleInfo021554f4* info;
    char pad8e1c[4];
    int turn;
};

struct BattleSub021554f4 {
    char data[0x48];
};

struct Battle021554f4 {
    char data[0x68];
    struct BattleSub021554f4 sub;
};

class GameState {
public:
    static GameState* GetInstance();
    struct Combatant021554f4* GetCombatantByIndex(int idx);
};

struct ArrayContainsByteStruct* GetPtrField0x2a04(GameState* gameState);
struct Combatant021554f4* GetCombatantWithFlag0x400(GameState* gameState, int id);
struct Combatant021554f4* GetCombatantWithFlag0x100(GameState* gameState, int id);
int IsFlag0x18Bit0x1000Set(GameObject* combatant);
int ArrayContainsByte(struct ArrayContainsByteStruct* arr, int value);
void IncrementByteCounterCapped0x63_0215a8d4(unsigned char* obj, int idx);
void* GetActiveSub_02054000(struct Outer_02054000* p);
int GetHalfwordEntryFromField150(struct Field150Holder02052df8* obj, int index);
void LoadBattleBlock020ac4c0(void* block);
void AddSaturateField0x44(struct S_a0840* p, unsigned int amount);
void AddClamped24BitFieldAt0x20(struct S_a02d0* p, unsigned int amount);
int GetField0x3acValue(GameState* gameState);
void AddClamped16BitHighAt0x24(struct S_a0644* p, unsigned int amount);
int CopyInBattleField0x7540(void* src);
extern "C" float func_ov000_02155a04(struct Combatant021554f4* combatant);

extern signed char data_ov000_02182a7c[];

static inline int IsPartySlot021554f4(int id) { return id >= 0 && id <= 3; }

// USA: func_ov000_021554f4
extern "C" ARM void func_ov000_021554f4(struct Root021554f4* root, int id, int killer, struct Source021554f4* source,
                                         unsigned char extra, int kind) {
    GameState* gs = GameState::GetInstance();
    struct ArrayContainsByteStruct* party = GetPtrField0x2a04(GameState::GetInstance());
    struct Combatant021554f4* c = GetCombatantWithFlag0x400(gs, id);
    if (c == 0) {
        return;
    }
    if (c->stats->currHP == 0 && kind == 8) {
        int index = root->lastIndex;
        if (index < 0) {
            return;
        }
        if (root->lastId == id) {
            root->entries[index].kind = kind;
        }
        return;
    }
    if (c->info != 0 && c->info->id == 0xf5 && IsFlag0x18Bit0x1000Set((GameObject*)c) &&
        ArrayContainsByte(party, c->stats->targetId) &&
        !(IsFlag0x18Bit0x1000Set((GameObject*)c) && c->guarded != 0) && c->countable != 0 &&
        ArrayContainsByte(party, killer)) {
        struct Combatant021554f4* k = GameState::GetInstance()->GetCombatantByIndex(killer);
        if (k != 0 && k->stats->buffs.charm == 2) {
            IncrementByteCounterCapped0x63_0215a8d4((unsigned char*)root, 8);
        }
    }
    root->lastId = -1;
    root->lastIndex = -1;
    if (root->entryCount >= 16) {
        return;
    }
    if (IsPartySlot021554f4(id)) {
        return;
    }
    root->lastId = id;
    root->lastIndex = root->entryCount;
    struct Entry021554f4* e;
    struct Info021554f4* info = c->info;
    struct Combatant021554f4* k = GameState::GetInstance()->GetCombatantByIndex(killer);
    e = &root->entries[root->entryCount++];
    memset(e, 0, sizeof(*e));
    unsigned int flags = c->stats->flags;
    if (c->guarded) {
        flags &= ~0x1000;
    }
    e->status = c->stats->status;
    e->flags = flags;
    e->colorA = c->stats->colorA;
    e->colorB = c->stats->colorB;
    e->colorC = c->stats->colorC;
    e->rank = c->stats->rank;
    e->style = c->stats->style;
    e->colorD = c->stats->colorD;
    e->buffs = c->stats->buffs;
    e->killerStatus = k->stats->status;
    e->killerFlags = k->stats->flags;
    e->killerColorA = k->stats->colorA;
    e->killerColorB = k->stats->colorB;
    e->killerColorC = k->stats->colorC;
    e->killerRank = k->stats->rank;
    e->killerStyle = k->stats->style;
    e->killerColorD = k->stats->colorD;
    e->killerBuffs = k->stats->buffs;
    e->infoId = info->id;
    e->killer = killer;
    e->kind = kind;
    e->extra = extra;
    e->turn = root->turn;
    e->ratio = func_ov000_02155a04(c);
    e->tier = 0;
    e->areaA = root->info->areaA;
    e->areaB = root->info->areaB;
    if (source != 0) {
        e->sourceId = source->id;
    }
    if (IsPartySlot021554f4(killer)) {
        struct Combatant021554f4* p = GetCombatantWithFlag0x100(gs, killer);
        if (p == 0) {
            return;
        }
        struct ActiveSub021554f4* sub = (struct ActiveSub021554f4*)GetActiveSub_02054000((struct Outer_02054000*)p);
        if (sub != 0) {
            e->tier = sub->tier;
        }
        for (int i = 0; data_ov000_02182a7c[i] != -1; i++) {
            e->skills[i] = GetHalfwordEntryFromField150((struct Field150Holder02052df8*)p, data_ov000_02182a7c[i]);
        }
    }
    if (!ArrayContainsByte(party, killer)) {
        return;
    }
    if ((kind & 1) || (kind & 2) || (kind & 4) || (kind & 0x10) || (kind & 0x20)) {
        return;
    }
    struct Battle021554f4 block;
    LoadBattleBlock020ac4c0(&block);
    AddSaturateField0x44((struct S_a0840*)&block, 1);
    AddClamped24BitFieldAt0x20((struct S_a02d0*)&block.sub, 1);
    if (killer == GetField0x3acValue(gs)) {
        AddClamped16BitHighAt0x24((struct S_a0644*)&block, 1);
    }
    CopyInBattleField0x7540(&block);
}
