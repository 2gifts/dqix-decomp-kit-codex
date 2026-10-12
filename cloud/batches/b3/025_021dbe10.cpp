#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"

struct Foo0207df50;
struct List02160094 { unsigned short kind; };
struct BattleSource { unsigned char padding0[0x20]; unsigned short id; };
struct S021d8a40;
struct ActionMetadata {
    unsigned char padding0[0x18];
    unsigned int flags : 5;
    unsigned int category : 7;
    unsigned int kind : 4;
    unsigned int other : 16;
};
struct ActionState {
    unsigned char padding0[0x30];
    SafeAllocator allocator;
    unsigned char padding_after_allocator[0x14c - 0x30 - sizeof(SafeAllocator)];
    unsigned char tables[0x150];
    char* battle;
    unsigned char padding2a0[8];
    char script[0xc04];
    int phase;
    unsigned char paddingeb0[0x4874];
    void* specialCombatant;
    unsigned char padding5728[0x229];
    unsigned char flags;
    unsigned char padding5952[0x14f9];
    unsigned char state;
    unsigned char padding6e4c[2];
    unsigned char field6e4e;
    unsigned char padding6e4f[0xad];
    short field6efc;
    unsigned char pending;
    unsigned char padding6eff[2];
    unsigned char field6f01;
    unsigned char padding6f02[0xb6];
    short field6fb8;
    unsigned char padding6fba[0x1a];
    unsigned char faded;
    unsigned char field6fd5;
    short source;
    unsigned char field6fd8;
    unsigned char padding6fd9[3];
    Vector3i position;
    unsigned char field6fe8;
    unsigned char padding6fe9[3];
    Vector3i offset;
    unsigned char field6ff8;
    unsigned char field6ff9;
    unsigned char padding6ffa[0x74a];
    unsigned char slots[3];
};
struct Timer021ef974 { unsigned char padding0[0xc]; int fieldc; };
extern "C" Timer021ef974 _ZZ16GetTimer021ef974vE1s;
extern char data_ov025_021ef40c[];
extern char data_ov025_021ef424[];
extern char data_ov025_021ef43c[];
extern int data_ov025_021ef404;
void CopyInternalFields0207df50(Foo0207df50*);
extern "C" void func_ov025_021dee20(ActionState*);
List02160094* GetSlotPtr02160f20(void*);
BattleSource* GetNodeAtIndex02160094(List02160094*, int);
void UpdateFlagAndDispatch_021dc0d8(void*);
void* GetField0Result02169a58(void*);
extern "C" void* func_ov025_021dfa9c(ActionState*, char*);
int ClassifyField0x81fe(char*);
extern "C" void* func_ov000_02169a58(void*, int);
extern "C" void func_ov025_021e88d8(void*);
void SetTwoFields_021e8a40(char*, int, int);
void SetField_021e8a4c(char*, int);
extern "C" void func_ov025_021e8d20(char*, void*);
extern "C" void func_ov025_021db8d8(ActionState*);
char* GetData02108e10();
ActionMetadata* SearchBothTables02079e2c(char*, int);
void ProcessPairs_021d8a40(void*, S021d8a40*);
void IncrementCounterAndReset_021def30(unsigned char*);
static inline int IsPartyIndex(unsigned int id) { return id <= 3; }

// USA: func_ov025_021dbe10
extern "C" ARM void func_ov025_021dbe10(ActionState* state) {
    GameState* game = GameState::GetInstance();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    char* battle = state->battle;
    state->allocator.Reset();
    CopyInternalFields0207df50((Foo0207df50*)state->tables);
    func_ov025_021dee20(state);
    List02160094* list = GetSlotPtr02160f20(state);
    if (!list) return;
    BattleSource* source = GetNodeAtIndex02160094(list, 0);
    if (!source) return;
    state->state = 0;
    state->field6e4e = 0;
    state->field6efc = -1;
    state->pending = 0;
    state->source = -1;
    state->field6fd8 = 0;
    state->field6fb8 = -1;
    state->field6f01 = 0;
    state->faded = 0;
    Vector3i zero = {};
    state->position = zero;
    state->field6fe8 = 0;
    state->offset = zero;
    state->field6ff8 = 0;
    state->field6ff9 = 0;
    state->slots[0] = 0xff;
    state->slots[1] = 0xff;
    state->slots[2] = 0xff;
    state->flags &= ~4;
    _ZZ16GetTimer021ef974vE1s.fieldc = 0;
    UpdateFlagAndDispatch_021dc0d8(state);
    int reset = 1;
    GameObject* object = game->GetCombatantByIndex(source->id);
    if (object) {
        int kind = list->kind;
        if (kind == 0x3ae) reset = 0;
        void* script = GetField0Result02169a58(object);
        void* overrideScript = func_ov025_021dfa9c(state, state->battle);
        if (overrideScript) script = overrideScript;
        if (ClassifyField0x81fe(battle) && source->id == 0)
            script = func_ov000_02169a58(state->specialCombatant, 0xdd);
        if (script) {
            func_ov025_021e88d8(state->script);
            SetTwoFields_021e8a40(state->script, (int)&state->allocator, (int)state->tables);
            SetField_021e8a4c(state->script, (int)state->battle);
            func_ov025_021e8d20(state->script, script);
            func_ov025_021db8d8(state);
        } else {
            ActionMetadata* metadata = SearchBothTables02079e2c(GetData02108e10(), (short)kind);
            if (metadata) {
                if (metadata->category == 0x22) ProcessPairs_021d8a40(state, (S021d8a40*)list);
                else {
                    char path[0x50];
                    if (metadata->kind == 2) sprintf(path, data_ov025_021ef40c);
                    else if ((metadata->kind == 1 || metadata->kind == 4) && IsPartyIndex(source->id))
                        sprintf(path, data_ov025_021ef424);
                    else sprintf(path, data_ov025_021ef43c);
                    data_ov025_021ef404 = loader->QueueLoadFile(path, 0);
                    state->phase = 4;
                }
            }
        }
    }
    if (reset) state->field6fd5 = 0;
    if (state->phase == 1) IncrementCounterAndReset_021def30((unsigned char*)state);
}
