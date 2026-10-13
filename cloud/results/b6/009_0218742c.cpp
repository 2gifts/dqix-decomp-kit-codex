#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"
#include "System/ColorEffects.h"
#include "Util/Random.h"
#include "std_library_functions.h"

struct PackedStats0218742c { unsigned int first : 10; unsigned int second : 10; unsigned int third : 10; unsigned int reserved : 2; };
struct Equipment0218742c { char bytes[32]; };
struct Character0218742c {
    PackedStats0218742c stats[4];
    char field_10[0x5c];
    unsigned short health;
    unsigned short mana;
    unsigned short field_70;
    unsigned short field_72;
    char field_74[0xc4];
    int experience[13];
    unsigned short levels[20];
    Equipment0218742c equipment[7];
    char field_274[0x214];
    short equipmentKeys[0x71];
    unsigned char appearance;
    char field_56b[0x3e5];
    int vocation;
};
struct Snapshot0218742c { char field_0[0x14c]; PackedStats0218742c stats[5]; char field_160[0xdc]; };
struct LoadedRecord0218742c { char field_0[0x3c]; char tail[0x18]; };
struct LearnedAbility0218742c { unsigned short id; unsigned short level; };
struct Script0218742c { char field_0[0x10c]; LearnedAbility0218742c abilities[13][20]; char field_51c[0x34]; };
struct ScriptNode0218742c { void* script; char field_4[0x1a]; unsigned char active; char field_1f; unsigned char waiting; };
struct Menu0218742c {
    char field_0[0xc0]; ScriptNode0218742c* script;
    char field_c4[4]; char equipmentContainer[0xf0];
    char display[0xfc]; char entryList[0x9a5];
    unsigned char step;
    char field_c5a[0x12a];
    signed char selection;
    char field_d85[3]; Character0218742c* character;
    char field_d8c[9]; unsigned char mode;
    char field_d96[6]; unsigned int flags;
    char field_da0[2]; unsigned char vocation; unsigned char selectedIndex;
    char field_da4[4]; unsigned char selections[1];
};
struct GameFlag0218742c { char field_0[0x63d4]; unsigned char changed; };
struct EquipmentMapping0218742c { unsigned char keyIndex; unsigned char destination; unsigned char field_2[5]; };
struct Struct_0205d81c; struct Obj0205eaa0; struct Struct_0205c570; struct Obj_0205da38;
struct Entry_0205d6a0; struct Obj0204b010; struct Cont0205d1e0; struct Cont0205d228;
struct Cont0205d274; struct Obj0205d2bc; struct Container020dedd0; struct Ctx0209a810;
void* GetDataPtr02114e04_020d6c00();
extern "C" void func_ov023_021d9f88(Menu0218742c*);
void SetElementFieldC2(Struct_0205d81c*, int, int);
void DispatchWithShortB4_0205eaa0(Obj0205eaa0*, int, int);
int GetActiveScaledSum0205d794(Struct_0205c570*);
extern "C" int _Z31IsActiveElementFlag2Set0205da38P12Obj_0205da38(Obj_0205da38*, int);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);
void ResetEntryList0205d6a0(Entry_0205d6a0*, int);
void ClearBuffer0204b010(Obj0204b010*, void*);
extern "C" void func_0204bc74(void*, int, int, int, int, int, int);
extern "C" void func_0204b04c(void*, int);
extern "C" void func_0204b088(void*, int);
void ClearBuffers0204b010OverList0x98(Cont0205d1e0*);
void CallFunc0204c8f0OverList0x9c(Cont0205d228*);
void CallFunc0204b04cOverList0x98(Cont0205d274*);
void InitEntries0205d2bc(Obj0205d2bc*);
void SetStatValue021855dc(unsigned char*, int);
extern "C" void func_02086404(Snapshot0218742c*);
void SetBit0x954StoreIndex0x950(unsigned char*, int);
extern "C" int sprintf(char*, const char*, ...);
void Clear0x54Bytes0208247c(void*);
void* LoadFileIntoMemory(const char*, void*, unsigned int*);
extern "C" void func_02082490(void*, void*, unsigned int, int, int);
extern "C" void func_02083cbc(void*, void*, void*);
void* FindElementByKey020dedd0(Container020dedd0*, int);
extern "C" void func_02083e28(void*, int);
void ResetAndCopyShorts0x1c(unsigned char*);
void ClearField00209a804(int*);
void SetupAndRunBufferedScript0209a810(Ctx0209a810*, void*);
void SetBitInArray0x910(unsigned char*, int);
extern "C" void func_020830cc(void*, Snapshot0218742c*);
extern "C" void func_02086778(void*, Snapshot0218742c*, unsigned char, unsigned char);
extern "C" void* func_ov003_0215f4fc(void*, const char*);
extern Obj0205eaa0 data_02108760;
extern unsigned short data_02114e30[];
extern unsigned char data_ov009_0218a9b4[];
extern char data_ov009_0218ad2c[];
extern char data_0211e33c[] __attribute__((aligned(4)));
extern EquipmentMapping0218742c data_ov009_0218aac8[];
extern char data_ov009_0218ad26[];

inline PackedStats0218742c* SnapshotStats(Snapshot0218742c* snapshot) {
    PackedStats0218742c* result = snapshot->stats;
    return result;
}

// USA: func_ov009_0218742c
extern "C" ARM void func_ov009_0218742c(Menu0218742c* menu) {
    int i;
    GameState* game = GameState::GetInstance();
    GetDataPtr02114e04_020d6c00();
    unsigned int step = menu->step;
    if (step == 0) {
        func_ov023_021d9f88(menu);
        SetElementFieldC2((Struct_0205d81c*)menu->entryList, 3, 2);
        DispatchWithShortB4_0205eaa0(&data_02108760, 5, 0);
        ++menu->step;
    } else if (step == 1) {
        menu->flags |= 0x2000;
        ColorEffect_ConfigureBrightnessAdjust(0x04001050, 1, -8);
        menu->flags &= ~0x1000000;
        ++menu->step;
    } else if (step == 2) {
        menu->selection = GetActiveScaledSum0205d794((Struct_0205c570*)menu->entryList);
        int cancel = 0;
        int active = _Z31IsActiveElementFlag2Set0205da38P12Obj_0205da38((Obj_0205da38*)menu->entryList, 0x14);
        int confirm = TestFlag0SetAndFlag1Clear(data_02114e30, 1);
        int alternate = TestFlag0SetAndFlag1Clear(data_02114e30, 0x400);
        if (active | (confirm | alternate)) {
            DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
            switch (menu->selection) {
            case 0: ++menu->step; break;
            case 1: cancel = 1; break;
            }
        } else if (TestFlag0SetAndFlag1Clear(data_02114e30, 2)) cancel = 1;
        if (menu->step == 3) {
            ResetEntryList0205d6a0((Entry_0205d6a0*)menu->entryList, 0);
            SetElementFieldC2((Struct_0205d81c*)menu->entryList, 3, 2);
            menu->flags &= ~0x60;
        } else if (cancel) {
            ResetEntryList0205d6a0((Entry_0205d6a0*)menu->entryList, 0);
            menu->flags &= ~0x60;
            menu->step = 255;
        }
    } else if (step == 3) {
        ClearBuffer0204b010((Obj0204b010*)menu->display, 0);
        func_0204bc74(menu->display, 0, 0, 0, 32, 25, 0);
        func_0204b04c(menu->display, 0);
        func_0204b088(menu->display, 0);
        if (menu->mode == 0) {
            Character0218742c* character = menu->character;
            character->appearance = data_ov009_0218a9b4[menu->selections[menu->selectedIndex]];
            ((GameFlag0218742c*)game)->changed = 1;
            ColorEffect_ConfigureBrightnessAdjust(0x04001050, 1, 0);
            SetElementFieldC2((Struct_0205d81c*)menu->entryList, 3, 0);
            ClearBuffers0204b010OverList0x98((Cont0205d1e0*)menu->entryList);
            CallFunc0204c8f0OverList0x9c((Cont0205d228*)menu->entryList);
            CallFunc0204b04cOverList0x98((Cont0205d274*)menu->entryList);
            InitEntries0205d2bc((Obj0205d2bc*)menu->entryList);
            SetStatValue021855dc((unsigned char*)menu, 10);
        } else if (menu->mode == 1) {
            Character0218742c* character = menu->character;
            void* members = GetPtrField0x2a04(game);
            Snapshot0218742c snapshot;
            func_02086404(&snapshot);
            SetBit0x954StoreIndex0x950((unsigned char*)character, menu->vocation);
            character->experience[menu->vocation] = 0;
            character->levels[menu->vocation] = 1;
            character->appearance = NextRandomMax(GetBTRandom(), 16);
            char filename[32];
            int characterVocation = character->vocation;
            sprintf(filename, data_ov009_0218ad2c, characterVocation);
            LoadedRecord0218742c record;
            Clear0x54Bytes0208247c(&record);
            BackgroundLoader::AddLockGlobal();
            unsigned int fileSize;
            void* file = LoadFileIntoMemory(filename, data_0211e33c, &fileSize);
            if (file) func_02082490(&record, file, fileSize, character->levels[menu->vocation], 0);
            func_02083cbc(character, &record, record.tail);
            BackgroundLoader::RemoveLockGlobal();
            short* keys = character->equipmentKeys;
            Equipment0218742c* equipment = character->equipment;
            for (i = 0; i < 7; ++i) {
                void* item = FindElementByKey020dedd0((Container020dedd0*)menu->equipmentContainer, keys[data_ov009_0218aac8[i].keyIndex]);
                if (item) memcpy(&equipment[data_ov009_0218aac8[i].destination], item, 32);
            }
            func_02083e28(character, 0);
            ResetAndCopyShorts0x1c((unsigned char*)character);
            PackedStats0218742c* stats = SnapshotStats(&snapshot);
            stats[0].first = character->stats[0].first;
            stats[0].second = character->stats[0].second;
            stats[0].third = character->stats[0].third;
            stats[1].first = character->stats[1].first;
            stats[1].second = character->stats[1].second;
            stats[1].third = character->stats[1].third;
            stats[2].first = character->stats[2].first;
            stats[2].second = character->health;
            stats[2].third = character->mana;
            stats[3].first = character->health;
            stats[3].second = character->mana;
            stats[3].third = character->field_70;
            stats[4].first = character->field_72;
            stats[4].second = character->stats[3].first;
            int scriptContext;
            ClearField00209a804(&scriptContext);
            Script0218742c script;
            SetupAndRunBufferedScript0209a810((Ctx0209a810*)&scriptContext, &script);
            unsigned char vocation = character->vocation;
            unsigned char level = character->levels[character->vocation];
            LearnedAbility0218742c* abilities = script.abilities[vocation];
            for (int i = 0; i < 20; ++i) {
                if (level >= abilities[i].level && abilities[i].level != 0)
                    SetBitInArray0x910((unsigned char*)character, abilities[i].id);
            }
            func_020830cc(character, &snapshot);
            func_02086778(members, &snapshot, 0, 0);
            menu->flags |= 0x400;
            SetStatValue021855dc((unsigned char*)menu, 11);
        }
    } else if (step == 255) {
        ClearBuffer0204b010((Obj0204b010*)menu->display, 0);
        func_0204bc74(menu->display, 0, 0, 0, 32, 25, 0);
        func_0204b04c(menu->display, 0);
        func_0204b088(menu->display, 0);
        ColorEffect_ConfigureBrightnessAdjust(0x04001050, 1, 0);
        SetElementFieldC2((Struct_0205d81c*)menu->entryList, 3, 0);
        SetElementFieldC2((Struct_0205d81c*)menu->entryList, 2, 0);
        void* script = func_ov003_0215f4fc(menu->script, data_ov009_0218ad26);
        if (script) menu->script->script = script;
        menu->script->active = 1;
        menu->script->waiting = 0;
        SetStatValue021855dc((unsigned char*)menu, 8);
        menu->step = 1;
    }
}

