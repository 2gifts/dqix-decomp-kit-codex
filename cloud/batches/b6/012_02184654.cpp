#include <globaldefs.h>
#include "GameState/GameState.h"
#include "std_library_functions.h"

struct Profile02184654 {
    unsigned int year : 12;
    unsigned int month : 4;
    unsigned int day : 5;
    unsigned int design : 4;
    unsigned int female : 1;
    unsigned int initialized : 1;
    unsigned int designChosen : 1;
    unsigned int birthdayChosen : 1;
    unsigned int accoladeChosen : 1;
    unsigned int vocationAccolade : 1;
    unsigned int showBirthday : 1;
    unsigned int age : 9;
    int title : 10;
    int accolade : 11;
    unsigned int flags : 2;
    char records[0x74];
};
struct Save02184654 { char field_0[0x569c]; Profile02184654 profile; };
struct Appearance02184654 {
    char field_0[0x414];
    unsigned char female : 1;
    unsigned char appearance : 7;
};
struct Party02184654 {
    char field_0[0x3c];
    char name[0x88-0x3c];
    Appearance02184654 appearance;
    char field_49d[0x950-0x49d];
    int vocation;
};
struct Member02184654 { char field_0[0x150]; Party02184654* party; };
struct TableA68 { char data[8]; };
struct Container020e0310 { char data[0x18]; };
struct Editor02184654 {
    char field_0[0x133c];
    TableA68 strings;
    Container020e0310 titles;
    char field_135c[0x54];
    char* name;
    char field_13b4[0x4c];
    char accoladeText[0x40];
    char titleText[0x40];
};
extern "C" void func_0202ae18();
Party02184654* GetFieldAt0x150(unsigned char*);
int CalculateAge(int, int, int);
const char* FindEntryByKey(TableA68*, int);
const char* GetFieldByKey020e0434(Container020e0310*, int);
extern "C" void func_ov023_021e66ac(Editor02184654*);

// USA: func_ov012_02184654
extern "C" ARM void func_ov012_02184654(Editor02184654* self) {
    Party02184654* party;
    Member02184654* protagonist;
    Profile02184654* profile;
    GameState* gameState = GameState::GetInstance();
    profile = &((Save02184654*)gameState)->profile;
    func_0202ae18();
    party = 0;
    protagonist = (Member02184654*)gameState->GetProtagonist();
    if (protagonist) party = GetFieldAt0x150((unsigned char*)protagonist);
    if (!party) return;
    self->name = party->name;
    if (!profile->initialized) {
        profile->year = 2000;
        profile->month = 1;
        profile->day = 1;
        profile->showBirthday = 1;
        Appearance02184654* appearance = &party->appearance;
        profile->female = appearance->female;
        profile->design = appearance->female;
        profile->title = 300;
        int accolade = 0x50dc;
        if (protagonist->party->appearance.female == 1) accolade += 50;
        profile->accolade = accolade + protagonist->party->vocation - 20000;
        memset(profile->records, 0, sizeof(profile->records));
        profile->initialized = 1;
    }
    profile->age = CalculateAge(profile->year, profile->month, profile->day);
    const char* text = FindEntryByKey(&self->strings, (short)(profile->title + 10000));
    memcpy(self->titleText, text, strlen(text));
    if (profile->accolade < 700) text = GetFieldByKey020e0434(&self->titles, (short)profile->accolade);
    else text = FindEntryByKey(&self->strings, (short)(profile->accolade + 20000));
    memcpy(self->accoladeText, text, strlen(text));
    func_ov023_021e66ac(self);
}
