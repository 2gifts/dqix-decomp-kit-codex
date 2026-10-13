#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"

struct Transition0215657c {
    char field_0;
    unsigned char done;
    char field_2[6];
    unsigned short destination;
    unsigned short state;
    char field_c[0xa];
    unsigned short currentId;
    unsigned short currentKind;
    char field_1a[0x7a];
    int context;
    char field_98[0xba];
    unsigned short targetId;
    unsigned short flag;
    unsigned short area;
    unsigned short targetKind;
};
struct Obj02046574 {
    char field_0[0x998];
    int active;
    char field_99c[0x1016];
    unsigned char field_19b2;
    char field_19b3[0xb];
    unsigned char field_19be;
};
struct Rgb02070900 { int red; int green; int blue; };
struct EffectParam02070900;
struct Timer020707c8;
struct EventFlags0215657c { char field_0[0x8c]; unsigned char flags[1]; };
char* GetFieldIfFlag4(char*);
extern "C" EventFlags0215657c* func_0205ec34();
int TestBitInByteArray(int, unsigned char*, int);
extern "C" unsigned short* func_02012fe4();
Obj02046574* GetGlobalField0x1c020421a0();
void SetIndexedName02046574(Obj02046574*, int, char*);
int CallFunc020e0434With02153694(int);
extern "C" void func_02046608(Obj02046574*, int, int, char*, int, int, int);
extern "C" void func_0204500c(Obj02046574*, char*, int, int);
void SetMainBrightness(GameResources*, int, int);
extern "C" EffectParam02070900* func_020704fc();
void SetColoredEffectParam02070900(EffectParam02070900*, Rgb02070900*, unsigned int);
void UpdateInterpTimer020707c8(Timer020707c8*);
extern "C" void func_0209c2e0(void*, int, int);
extern "C" void func_02046a8c(void*, void*);
void ClearFields_021bb0c4(unsigned char*);
void SetBrightness(GameResources*, int, int);
void SetField0x238True(void*);
void ApplyVecFromField0x246(char*);
extern "C" void func_ov017_021bb27c(void*);
extern int data_02109bf4 __attribute__((aligned(4)));

// USA: func_ov001_0215657c
extern "C" ARM int func_ov001_0215657c(Transition0215657c* transition, void* list) {
    if (BackgroundLoader::GetInstance()->GetNumQueuedTasks() > 0) return transition->state;
    GameState* game = GameState::GetInstance();
    GameResources* resources = func_ov017_0218b5b0();
    game->GetUnknownGameObject();
    char* field = GetFieldIfFlag4((char*)game);
    int blocked = 0;
    int sameArea = 0;
    void* node = resources->unknown_ptr_3718;
    if (transition->currentId == transition->targetId) blocked = 1;
    else if (transition->currentKind == transition->targetKind) blocked = 1;
    else {
        EventFlags0215657c* flags = func_0205ec34();
        if (TestBitInByteArray((int)flags, flags->flags, transition->flag + 0x38e)) blocked = 1;
        else if (transition->area == *func_02012fe4()) sameArea = 1;
    }
    if (blocked) {
        GameState* state = GameState::GetInstance();
        GameResources* display = func_ov017_0218b5b0();
        Obj02046574* message = GetGlobalField0x1c020421a0();
        GameObject* member = GetCombatantWithFlag0x100(state, 0);
        if (member) {
            SetIndexedName02046574(message, 0, (char*)member->baseStats_);
            char text[0x100];
            
            int messageId = CallFunc020e0434With02153694(0x34);
            func_02046608(message, 12, messageId, text, 0xe3, 0, 1);
            func_0204500c(message, text, 0, 0xe3);
            message->active = 1;
            message->field_19b2 = 0;
            message->field_19be = 1;
        }
        SetMainBrightness(display, 0, 1);
        EffectParam02070900* effect = func_020704fc();
        Rgb02070900 color = {};
        SetColoredEffectParam02070900(effect, &color, 0);
        UpdateInterpTimer020707c8((Timer020707c8*)effect);
        return 5;
    }
    if (sameArea) {
        transition->destination = transition->targetId;
        func_0209c2e0(&data_02109bf4, transition->context, 0);
        func_02046a8c(list, node);
        ClearFields_021bb0c4((unsigned char*)transition);
        return 0;
    }
    SetBrightness(resources, 0, 30);
    SetField0x238True(field);
    ApplyVecFromField0x246(field);
    func_0209c2e0(&data_02109bf4, transition->context, 0);
    func_ov017_021bb27c(transition);
    transition->done = 1;
    func_02046a8c(list, node);
    return transition->state;
}
