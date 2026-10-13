#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/Brightness.h"
#include "Filesystem/BackgroundLoader.h"
#include "Filesystem/FileIO.h"
#include "std_library_functions.h"

struct Menu02186388 {
    char field_0[0xa0]; char table[0x18]; char* text;
    char field_bc[0x74]; char list[0x614]; unsigned char selectionCodes[8]; unsigned char count;
    char field_74d[3]; char selection[0x3c0]; unsigned char mode; unsigned char step;
    char field_b12[2]; short previousIndex; char field_b16[2]; unsigned int flags;
    char field_b1c[0xd]; unsigned char scroll;
};
struct Formatter02186388 {
    char field_0[0x99c]; int mode; int state; char field_9a4[0x100a]; unsigned char field_19ae;
    char field_19af[3]; unsigned char field_19b2; char field_19b3[0x12]; unsigned char field_19c5;
    char field_19c6[2]; unsigned char field_19c8;
};
struct Entry02186388 { char field_0[0xc2]; short field_c2; };
struct Region02186388 { char field_0[0x10]; unsigned int field_10 : 23; unsigned int index : 9; };
struct Save02186388 { char field_0[0x75d0]; Region02186388 region; };
struct Character02186388 { char field_0[0x49c]; unsigned char female : 1; unsigned char field_49c : 7; };
struct Actor02186388 { char field_0[0x150]; Character02186388* character; };
struct List020727d8 { void* entries; short capacity; short count; };
struct StreamHeader; struct Manager02187664; struct Struct_0205d81c; struct Container020e0310; struct Obj0205eaa0;
int GetWord0x0(int*);
Formatter02186388* GetGlobalField0x1c020421a0();
extern "C" int func_ov008_02188a54(Menu02186388*, int);
extern "C" void func_ov008_0218712c(Menu02186388*, Region02186388*);
void InitConfig02187664(Manager02187664*, int);
extern "C" void func_ov008_02187a70(Menu02186388*, Region02186388*);
extern "C" void func_ov008_02187f6c(Menu02186388*, Region02186388*);
Entry02186388* FindElementByC40205d81c(Struct_0205d81c*, int);
void ResetListHeader020727d8(List020727d8*);
void SubmitAndRunScript02072928(void*, StreamHeader*, int, int, void*, unsigned char);
int GetField0x3acValue(GameState*);
int GetFieldByKey020e0434(Container020e0310*, int);
int AppendString02042058(char*, const char*);
extern "C" void func_0204500c(Formatter02186388*, char*, int, int);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);
void SelectCoordsByFlag0x24(unsigned char*, int*, int*);
extern "C" int func_ov008_02186b18(Menu02186388*, int, int);
void DispatchWithShortB4_0205eaa0(Obj0205eaa0*, int, int);
extern "C" void func_ov008_02186964(Menu02186388*);
extern "C" void func_0205bb04(void*, int);
extern char data_ov008_0218b400[];
extern unsigned short data_02114e30;
extern unsigned char data_02114e54[];
extern Obj0205eaa0 data_02108760;

// USA: func_ov008_02186388
extern "C" ARM void func_ov008_02186388(Menu02186388* menu) {
    GameState* game = GameState::GetInstance();
    GameResources* resources = (GameResources*)GetWord0x0((int*)game);
    BackgroundLoader::GetInstance();
    Formatter02186388* formatter = GetGlobalField0x1c020421a0();
    Region02186388* region = &((Save02186388*)game)->region;
    formatter->field_19ae = 0;
    unsigned char step = menu->step;
    if (step == 0) { SetSubBrightness(resources, -16, 15); ++step; ++menu->step; }
    if (step == 1) {
        if (func_ov008_02188a54(menu, 1)) {
            func_ov008_0218712c(menu, region);
            InitConfig02187664((Manager02187664*)menu, (int)region);
            func_ov008_02187a70(menu, region);
            func_ov008_02187f6c(menu, region);
            for (int i = 0; i < 5; ++i) {
                Entry02186388* entry = FindElementByC40205d81c((Struct_0205d81c*)menu->list, (unsigned char)i);
                if (entry) entry->field_c2 = 0;
            }
            menu->flags |= 0x10000;
            ++menu->step;
        }
    }
    if (step == 2) { SetSubBrightness(resources, 0, 15); ++menu->step; }
    if (step == 3) {
        if (IsBrightnessTransitionActive(resources)) return;
        if (menu->previousIndex != region->index) {
            memset(menu->text, 0, 0x960);
            char* message;
            char* archiveName;
            char* fileName;
            archiveName = menu->text;
            fileName = menu->text + 0x80;
            message = menu->text + 0x100;
            BackgroundLoader::AddLockGlobal();
            List020727d8 list;
            unsigned int fileSize;
            StreamHeader* file = (StreamHeader*)LoadFileIntoMemory(data_ov008_0218b400, data_0211e33c, &fileSize);
            if (file) {
                ResetListHeader020727d8(&list);
                SubmitAndRunScript02072928(&list, file, fileSize, 1001, archiveName, 0);
                ResetListHeader020727d8(&list);
                SubmitAndRunScript02072928(&list, file, fileSize, 1001, fileName, 1);
            }
            StreamHeader* script = (StreamHeader*)ExtractFileFromGP2(archiveName, fileName, &fileSize);
            if (script) {
                int group = GetField0x3acValue(game);
                Actor02186388* actor = (Actor02186388*)GetCombatantWithFlag0x100(game, group);
                ResetListHeader020727d8(&list);
                SubmitAndRunScript02072928(&list, script, fileSize, region->index, message, actor->character->female);
            }
            BackgroundLoader::RemoveLockGlobal();
            char* suffix = (char*)GetFieldByKey020e0434((Container020e0310*)menu->table, 1000);
            AppendString02042058(message, suffix);
            func_0204500c(formatter, message, 0, 0xe3);
            formatter->field_19b2 = 1;
            formatter->field_19c8 = 1;
            formatter->mode = 2;
            int previouslyShown = 1;
            if (!(menu->flags & 0x40000)) previouslyShown = 0;
            if (previouslyShown) { formatter->field_19c5 = 1; formatter->field_19b2 = 0; }
            else formatter->field_19c5 = 0;
            menu->flags |= 0x40000;
        }
        ++step; ++menu->step;
    }
    if (step == 4) {
        if (formatter->state != 3) return;
        int cancel = 0;
        if (TestFlag0SetAndFlag1Clear(&data_02114e30, 0x403)) cancel = 1;
        int x, y;
        SelectCoordsByFlag0x24(data_02114e54, &x, &y);
        if (data_02114e54[0x55]) {
            int index = func_ov008_02186b18(menu, x, y);
            if (index == -2) cancel = 1;
            if (menu->selectionCodes[index] == 7) cancel = 1;
        }
        if (cancel) { DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0); ++menu->step; }
    }
    if (step == 5) { SetSubBrightness(resources, -16, 15); ++menu->step; }
    if (step == 6) {
        if (func_ov008_02188a54(menu, 0)) {
            func_ov008_0218712c(menu, 0);
            InitConfig02187664((Manager02187664*)menu, 0);
            func_ov008_02187a70(menu, 0);
            func_ov008_02187f6c(menu, 0);
            for (int i = 0; i < 6; ++i) {
                Entry02186388* entry = FindElementByC40205d81c((Struct_0205d81c*)menu->list, (unsigned char)i);
                if (entry) entry->field_c2 = 0;
            }
            menu->flags &= ~0x10000;
            ++menu->step;
        }
    }
    if (step == 7) { SetSubBrightness(resources, 0, 15); ++menu->step; }
    if (step == 8) {
        if (IsBrightnessTransitionActive(resources)) return;
        if (menu->previousIndex == region->index) menu->flags |= 0x200;
        else { menu->flags |= 0x20000; menu->flags &= ~0x200; }
        menu->mode = 3;
        menu->step = 2;
        func_ov008_02186964(menu);
        for (int i = 0; i < menu->count; ++i) {
            if (menu->selectionCodes[i] == 7) {
                func_0205bb04(menu->selection, i);
                if (i > menu->scroll + 5) { menu->scroll = i - 5; return; }
                if (i < menu->scroll) menu->scroll = i;
                break;
            }
        }
    }
}
