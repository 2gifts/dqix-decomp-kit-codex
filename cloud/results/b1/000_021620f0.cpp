#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Util/Random.h"

struct Container02070e60;
struct BinarySearchByComparatorStruct;
struct FieldStruct02048934 {
    unsigned char pad_0[0x2c];
    unsigned char payload[1];
};
struct Obj02048850;
struct Param02048850;

struct Stats021620f0 {
    unsigned short field_0;
    unsigned short field_2;
    unsigned short field_4;
    unsigned short field_6;
    unsigned char pad_8[0x1d];
    unsigned char slot;
    unsigned char pad_26[0xa4 - 0x26];
};

struct Combatant021620f0 {
    Object3D object;
    unsigned char pad_ac[0x134 - 0xac];
    unsigned char* payload;
    Stats021620f0* stats;
    unsigned char pad_13c[0xc];
    FieldStruct02048934* entry;
    unsigned char pad_14c[0x30];
    unsigned char mode;
    unsigned char pad_17d[7];
    unsigned char active;
    unsigned char pad_185;
    unsigned short kind;
    unsigned short field_188;
    unsigned short field_18a;
    unsigned char group;
    unsigned char slot;
    unsigned char pad_18e[2];
};

struct Model021620f0 { unsigned char data[0x70]; };

struct Roster021620f0 {
    unsigned char pad_0[0x158];
    Stats021620f0 stats[8];
    unsigned char infos[0xc];
    unsigned char names[0xc];
};

struct Group021620f0 {
    unsigned short kind;
    unsigned char ids[8];
    unsigned char count : 4;
    unsigned char used : 4;
    unsigned char pad_b[2];
    unsigned char names[8];
    unsigned char nameCount;
    unsigned char pad_16[2];
};

struct Pair021620f0 {
    unsigned char next;
    unsigned char pad_1;
    unsigned short kind;
    unsigned int slots;
};

struct Formation021620f0 {
    unsigned char total;
    unsigned char alive : 4;
    unsigned char groupCount : 2;
    unsigned char pairCount : 2;
    unsigned short field_2;
    Group021620f0 groups[3];
    Pair021620f0 pairs[4];
};

struct BattleData021620f0 {
    Random random;
    unsigned char pad_20[0x81b0 - sizeof(Random)];
    Formation021620f0 formation;
};

struct Scene021620f0 {
    unsigned char pad_0[0x29c];
    BattleData021620f0* data;
    Roster021620f0* roster;
    unsigned char pad_2a4[0xec8 - 0x2a4];
    unsigned short field_ec8;
    unsigned char pad_eca[0x5e24 - 0xeca];
    Combatant021620f0 combatants[8];
    Model021620f0 models[8];
};

extern "C" void* func_0202ae18(void);
extern "C" GameObject* _Z25GetCombatantWithFlag0x400P9GameStatei(GameState* gameState, int combatantId);
extern "C" void _Z18ZeroStruct021620d8P22ZeroableStruct021620d8(Pair021620f0* pair);
extern "C" FieldStruct02048934* _Z33SearchWithLow15Comparator02070fd0P17Container02070e60i(Container02070e60* table, int key);
extern "C" Param02048850* _Z28SearchWithComparator0206f4f0P30BinarySearchByComparatorStructi(BinarySearchByComparatorStruct* table, int key);
extern "C" void _Z24FindOrAssignSlot02167c28Phi(unsigned char* obj, int id);
extern "C" int func_ov000_02161bb8(unsigned char* obj);
extern "C" void func_020891cc(void* obj);
extern "C" void func_02089630(void* dst, void* src, int useExactValue);
extern "C" void func_02049c88(void* obj, void* model);
extern "C" void _Z21RegisterCombatantSlotP9GameStateiP10GameObject(GameState* gameState, int id, GameObject* combatant);
extern "C" void _Z21SyncNameFlag_02161274P10GameObject(GameObject* obj);
void SetRandomizedShort0x18802048934(void* p, FieldStruct02048934* f, Random* r);
void IncrementByteCounterWrapAt0x12(unsigned char* p);
void ResetNameState02048614(unsigned char* obj);
void SetupFieldsFromParam02048850(Obj02048850* obj, Param02048850* param);
void BuildName020488ec(char* obj);
void ClearSubstructByte0x56(unsigned char* obj);

extern const short data_ov000_02182ff8[][3];
extern const signed char data_ov000_02182fd4[][3];

static inline unsigned char GetStatsSlot(Combatant021620f0* c) { unsigned char slot = c->stats->slot; return slot; }

static inline Random* GetRandom(Scene021620f0* scene) { Random* random = &scene->data->random; return random; }

// USA: func_ov000_021620f0
extern "C" ARM void func_ov000_021620f0(Scene021620f0* scene, int formation) {
    unsigned short kind;
    int id;
    Combatant021620f0* c;
    Formation021620f0* f = &scene->data->formation;
    GameState* game = GameState::GetInstance();
    func_0202ae18();
    Container02070e60* names = (Container02070e60*)scene->roster->names;
    BinarySearchByComparatorStruct* infos = (BinarySearchByComparatorStruct*)scene->roster->infos;
    f->total = 0;
    f->alive = 0;
    f->groupCount = 0;
    f->pairCount = 0;
    f->field_2 = 0;
    for (int i = 0; i < 3; i++) {
        memset(&f->groups[i], 0, sizeof(Group021620f0));
    }
    for (int i = 0; i < 4; i++) {
        _Z18ZeroStruct021620d8P22ZeroableStruct021620d8(&f->pairs[i]);
    }
    const short* kinds = data_ov000_02182ff8[formation];
    const signed char* counts = data_ov000_02182fd4[formation];
    int g;
    for (g = 0, id = 0; kinds[g] != -1; g++) {
        FieldStruct02048934* entry = _Z33SearchWithLow15Comparator02070fd0P17Container02070e60i(names, kinds[g]);
        Param02048850* info = _Z28SearchWithComparator0206f4f0P30BinarySearchByComparatorStructi(infos, kinds[g]);
        if (!entry || !info) return;
        int key = kinds[g];
        kind = key;
        f->groups[g].kind = kind;
        f->pairs[g].kind = kind;
        f->pairs[g].next = 1;
        f->groupCount++;
        f->pairCount++;
        for (int j = 0; j < counts[g]; j++, id++) {
            c = (Combatant021620f0*)_Z25GetCombatantWithFlag0x400P9GameStatei(game, id + 0xc0);
            Stats021620f0* stats = &scene->roster->stats[id];
            func_020891cc(stats);
            if (!c) {
                _Z24FindOrAssignSlot02167c28Phi((unsigned char*)scene, id + 0xc0);
                c = &scene->combatants[id];
                ResetNameState02048614((unsigned char*)c);
                func_02049c88(c, &scene->models[id]);
                func_02089630(stats, entry->payload, 0);
                c->object.SetField06(scene->field_ec8);
                c->stats = stats;
                c->payload = entry->payload;
                c->entry = entry;
                SetupFieldsFromParam02048850((Obj02048850*)c, info);
                c->object.MakeHidden();
                ClearSubstructByte0x56((unsigned char*)c);
                _Z21RegisterCombatantSlotP9GameStateiP10GameObject(game, id + 0xc0, (GameObject*)c);
            }
            SetRandomizedShort0x18802048934(c, entry, GetRandom(scene));
            unsigned short v0 = c->field_188;
            unsigned short v2 = c->field_18a;
            c->stats->field_0 = v0;
            c->stats->field_4 = v0;
            c->stats->field_2 = v2;
            c->stats->field_6 = v2;
            unsigned char slot = func_ov000_02161bb8((unsigned char*)&f->pairs[g]);
            if (slot == 1) {
                slot = 0;
            } else if (slot == 2) {
                _Z21SyncNameFlag_02161274P10GameObject((GameObject*)c);
            }
            c->object.unknown_2_ = key;
            c->mode = g;
            c->stats->slot = slot;
            BuildName020488ec((char*)c);
            c->kind = kind;
            c->group = g;
            c->slot = slot;
            c->active = 1;
            int bit = GetStatsSlot(c);
            f->pairs[g].slots |= 1 << bit;
            IncrementByteCounterWrapAt0x12((unsigned char*)&f->pairs[g]);
            f->groups[g].ids[j] = id;
            f->groups[g].count++;
            f->groups[g].used++;
            f->total++;
            f->alive++;
        }
    }
}
