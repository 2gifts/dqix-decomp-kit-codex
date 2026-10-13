#include <globaldefs.h>
#include "GameState/GameState.h"

struct BgControl { short value; };
struct ArenaState {
    char pad0[0x36fc];
    void** list;
    char pad3700[0x980];
    int blocked;
    char pad4084[4];
    BgControl* bg1Backup;
    BgControl* bg2Backup;
    char pad4090[0x130];
    int initialized;
};
struct FieldContext { char pad0[0x998]; int blocked; };
struct ZoneData { char pad0[0xc]; unsigned char mode : 4; unsigned char other : 4; };
struct Something02015a2c { char pad0[8]; ZoneData* zone; };
extern "C" void* func_0202ae18();
int GetIntField0x260(void*);
FieldContext* GetGlobalField0x1c020421a0();
extern "C" Something02015a2c* func_02012fe4();
void* GetGlobalContext020daf90();
int IsField0Null(void**);
extern "C" int func_0202c540(void*);
int IsGlobalU16InRange(GameState*);
int CheckSlotsForFlagState02015a2c(Something02015a2c*);
int GetField0x50(void*);
extern "C" void func_ov017_021a5bac(ArenaState*);
void InitFieldArenaLists_021a5b48(unsigned char*);
extern "C" void func_ov017_021a5cbc(ArenaState*);
#define REG_BG1CNT (*(volatile unsigned short*)0x0400000a)
#define REG_BG2CNT (*(volatile unsigned short*)0x0400000c)
static inline BgControl BackupBg1(BgControl* backup) { BgControl control; control.value = REG_BG1CNT; *backup = control; return control; }
static inline BgControl BackupBg2(BgControl* backup) { BgControl control; control.value = REG_BG2CNT; *backup = control; return control; }

// USA: func_ov017_021a624c
extern "C" ARM void func_ov017_021a624c(ArenaState* self) {
    void* search = func_0202ae18();
    GameState* game = GameState::GetInstance();
    GameObject* hero = game->GetProtagonist();
    if (!hero) return;
    int action = GetIntField0x260(hero);
    FieldContext* field = GetGlobalField0x1c020421a0();
    Something02015a2c* world = func_02012fe4();
    GrottoStruct* grotto;
    ZoneData* zone = world->zone;
    grotto = game->GetGrottoStruct();
    void* context = GetGlobalContext020daf90();
    if (!game->GetGrottoStruct()->unknown_0[0] || self->blocked || action != -1
        || !IsField0Null(self->list) || func_0202c540(search)
        || IsGlobalU16InRange(GameState::GetInstance()) || field->blocked
        || CheckSlotsForFlagState02015a2c(world) || !zone || zone->mode || GetField0x50(context)) {
        grotto->unknown_0[2] = 1;
        func_ov017_021a5bac(self);
    } else if (!self->initialized) {
        BackupBg1(self->bg1Backup);
        BackupBg2(self->bg2Backup);
        grotto->unknown_0[2] = 0;
        InitFieldArenaLists_021a5b48((unsigned char*)self);
        self->initialized = 1;
    } else {
        func_ov017_021a5cbc(self);
    }
}
