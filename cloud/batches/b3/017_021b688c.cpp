#include <globaldefs.h>
#include <GameState/GameState.h>
#include <Memory/SafeAllocator.h>

struct Obj020397cc;
struct ListHead02046b60;
struct MenuData {
    char pad_0[0x330];
    unsigned int displayMode;
    char pad_334[0x480 - 0x334];
    short selection;
    char pad_482[0x4a1 - 0x482];
    unsigned char flags;
    char pad_4a2[0x22];
};
struct MenuWork {
    unsigned char field_0;
    unsigned char complete;
    char pad_2[6];
    SafeAllocator allocator;
    unsigned char mode;
    unsigned char state;
    unsigned char field_1e;
    unsigned char flags;
    signed char selection;
    char pad_21[3];
    MenuData* data;
};
struct PartySlots { char pad_0[0xf78]; signed char slots[4]; unsigned char count; };
void CancelPendingAction020397cc(Obj020397cc*, int);
int GetWord0x0(int*);
void SetBitsInField4(unsigned int*, unsigned int);
int GetFieldIfFlag4(char*);
void SetFlagsAt0x244(unsigned char*, unsigned char);
extern "C" void func_020a0cc4(int);
void* AllocateAligned4(AllocatorUnion*, unsigned int);
extern AllocatorUnion data_02114e20;
extern "C" void func_020a0c0c();
void PushInputLogA(int);
extern "C" void func_ov003_0215f9c0(MenuData*, int);
extern "C" void func_ov003_0215f8cc(MenuData*, SafeAllocator*);
void ClearIntAt0x23c(unsigned char*);
extern "C" int func_ov003_0215fce8(MenuData*, unsigned int);
void* GetGlobal02109400();
void BlankFunction02094b40();
extern "C" void func_02094ab0(void*);
void BlankFunction02094b34();
int AlwaysTrue02094b4c();
extern "C" void func_ov003_0215fb94(MenuData*);
extern "C" void func_ov017_021b67d8(MenuWork*);
int ListContainsId(ListHead02046b60*, int);
void ClearBitsInField4(unsigned int*, unsigned int);
void SetByteField0x253(void*);
extern "C" int func_ov017_021959b4();
void SetField0x23cTrue(void*);
void ClearFlagBits(unsigned char*, int);
extern "C" void* func_0205ec34();
void* GetPtrField0x2a04(GameState*);
extern "C" void func_0206e424(void*, int);
#define DISPLAY_CONTROL (*(volatile unsigned int*)0x04000000)

// USA: func_ov017_021b688c
extern "C" ARM void func_ov017_021b688c(MenuWork* work) {
    GameState* game = GameState::GetInstance();
    Obj020397cc* object = (Obj020397cc*)game->GetUnknownGameObject();
    CancelPendingAction020397cc(object, 1);
    GameResources* resources = (GameResources*)GetWord0x0((int*)game);
    SetBitsInField4((unsigned int*)resources, 0xc0);
    SetFlagsAt0x244((unsigned char*)GetFieldIfFlag4((char*)game), 1);
    if (work->state == 0) {
        func_020a0cc4(0x1b800);
        void* memory = AllocateAligned4(&data_02114e20, 0x1b800);
        if (!memory) {
            func_020a0c0c();
            work->complete = 1;
            return;
        }
        work->allocator.CreateTypeA(memory, 0x1b800);
        work->allocator.Reset();
        work->data = (MenuData*)work->allocator.Allocate(0x4c4);
        if (!work->data) {
            func_020a0c0c();
            work->complete = 1;
            return;
        }
        PushInputLogA(3);
        func_ov003_0215f9c0(work->data, work->mode);
        work->data->flags = work->flags;
        work->data->selection = work->selection;
        func_ov003_0215f8cc(work->data, &work->allocator);
        ClearIntAt0x23c((unsigned char*)GetFieldIfFlag4((char*)game));
        work->state++;
    } else if (work->state == 1) {
        if (!func_ov003_0215fce8(work->data, game->GetTickCount())) return;
        void* global = GetGlobal02109400();
        BlankFunction02094b40();
        func_02094ab0(global);
        BlankFunction02094b34();
        work->state++;
    } else if (work->state == 2) {
        GetGlobal02109400();
        if (!AlwaysTrue02094b4c()) return;
        func_ov003_0215fb94(work->data);
        work->state++;
    } else if (work->state == 3) {
        DISPLAY_CONTROL = (DISPLAY_CONTROL & ~0x1f00) | (work->data->displayMode << 8);
        work->state++;
        func_ov017_021b67d8(work);
        work->complete = 1;
        ListHead02046b60* list = (ListHead02046b60*)resources->unknown_ptr_array_36fc[0];
        if (ListContainsId(list, 4) || ListContainsId(list, 0x18)) {
            CancelPendingAction020397cc(object, 1);
            return;
        }
        ClearBitsInField4((unsigned int*)resources, 0xc0);
        SetByteField0x253(object);
        unsigned char* flags = (unsigned char*)GetFieldIfFlag4((char*)game);
        if (!func_ov017_021959b4()) SetField0x23cTrue(flags);
        ClearFlagBits(flags, 2);
        void* party = func_0205ec34();
        GetPtrField0x2a04(game);
        PartySlots* slots = (PartySlots*)GetPtrField0x2a04(game);
        for (int i = 0; i < slots->count; i++) func_0206e424(party, slots->slots[i]);
    }
}
