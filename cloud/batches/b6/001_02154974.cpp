#include <globaldefs.h>
#include "GameState/GameState.h"

struct CommandNode02154974 {
    unsigned char kind : 4;
    unsigned char flags : 4;
    unsigned char id;
    short objectId;
    int field_4;
    int key;
    char field_c[5];
    char name[0x23];
    CommandNode02154974* next;
};
struct AnimationNode02154974 { unsigned char id; char field_1[0x27]; AnimationNode02154974* next; };
struct Cleanup02154974 {
    char field_0[0xd0];
    CommandNode02154974* commands;
    AnimationNode02154974* animations;
    char field_d8[0x20];
    unsigned int displayFlags : 27;
    unsigned int field_f8 : 5;
};
struct EntryActor02154974 { char field_0[0xd4]; unsigned char flags; };
struct ResourceEntry02154974 { unsigned int flags; char field_4[0x14]; EntryActor02154974* actor; };
struct Manager02154974 { char field_0[0x98]; unsigned int flags; };
struct EntryList_203dce4;
struct Container02040404;
struct Container0204039c;
struct SlotArray0203dd10;
Manager02154974* GetGlobalPtr021075f4();
ResourceEntry02154974* FindEntryPointerByKey0203df78(void*, int);
void ClearCombatantSlot(GameState*, int);
int* GetEntryUnlessFlag0x8000(EntryList_203dce4*, int);
void EmptyStub0203d004();
int FindEntryIndexByName02040404(Container02040404*, const char*);
void ReleaseOrDecrementSlot0204039c(Container0204039c*, int);
void Reset02040504(int*);
void SetSlotIfIndexBelow32(SlotArray0203dd10*, unsigned int, int);
void CallByIndex_0215acd4(int);
void ClearBitsInField4(unsigned int*, unsigned int);
extern char data_ov001_02164bd1[];

// USA: func_ov001_02154974
extern "C" ARM void func_ov001_02154974(Cleanup02154974* cleanup) {
    GameState* game = GameState::GetInstance();
    Manager02154974* manager = GetGlobalPtr021075f4();
    GameResources* resources = func_ov017_0218b5b0();
    for (AnimationNode02154974* animation = cleanup->animations; animation; animation = animation->next) {
        CommandNode02154974* command = cleanup->commands;
        while (command) {
            if (command->id == animation->id) {
                int kind = command->kind;
                if (kind == 0 || kind == 1 || kind == 9 || kind == 11 || kind == 12 || kind == 13 || kind == 10 || kind == 4) {
                    GameObject* member = game->GetGameObjectByIndex(command->objectId);
                    if (member) {
                        int flags = member->obj3D_.animationFlags_;
                        const char* name = (const char*)member->obj3D_.activeAnimationRecord_;
                        member->obj3D_.RemoveAnimationPackageByID(3);
                        if (!member->obj3D_.MaybeSetRegularAnimation(name, flags)) member->obj3D_.MaybeSetRegularAnimation(data_ov001_02164bd1, 0);
                    }
                }
                break;
            }
            command = command->next;
        }
    }
    for (CommandNode02154974* command = cleanup->commands; command; command = command->next) {
        switch (command->kind) {
        case 5: {
            ResourceEntry02154974* entry = FindEntryPointerByKey0203df78(manager, command->key);
            if (entry) entry->flags &= ~0x10000;
            break;
        }
        case 4: {
            ClearCombatantSlot(game, command->objectId);
            ResourceEntry02154974* entry = FindEntryPointerByKey0203df78(manager, command->key);
            if (entry && entry->actor) {
                entry->actor->flags &= ~1;
                entry->flags &= ~0x10000;
            }
            break;
        }
        case 2:
        case 6:
        case 7:
        case 8:
            if (game->GetGameObjectByIndex(command->objectId)) ClearCombatantSlot(game, command->objectId);
            break;
        case 3: {
            int* entry = GetEntryUnlessFlag0x8000((EntryList_203dce4*)manager, command->objectId);
            if (entry) {
                EmptyStub0203d004();
                Container02040404* container = (Container02040404*)manager;
                int index = FindEntryIndexByName02040404(container, command->name);
                ReleaseOrDecrementSlot0204039c((Container0204039c*)container, index);
                Reset02040504(entry);
                SetSlotIfIndexBelow32((SlotArray0203dd10*)manager, command->objectId, 0);
            }
            break;
        }
        case 0:
        case 1:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            break;
        }
        CallByIndex_0215acd4(command->id);
    }
    if (cleanup->displayFlags) {
        if (cleanup->displayFlags & 1) ClearBitsInField4((unsigned int*)resources, 8);
        if (cleanup->displayFlags & 2) ClearBitsInField4((unsigned int*)resources, 16);
        if (cleanup->displayFlags & 4) {
            for (int i = 0; i < 4; ++i) {
                GameObject* member = game->GetGameObjectByIndex(i);
                if (member) member->obj3D_.DisableFlag(1);
            }
        }
        if (cleanup->displayFlags & 8) ClearBitsInField4((unsigned int*)resources, 0x400);
        if (cleanup->displayFlags & 0x20) GetGlobalPtr021075f4()->flags &= ~2;
    }
}
