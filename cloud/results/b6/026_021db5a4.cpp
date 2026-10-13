#include <globaldefs.h>
#include <GameState/GameState.h>
#include <Filesystem/BackgroundLoader.h>
#include <Memory/SafeAllocator.h>

struct BattleState;
struct PartyState;
struct Container_021dae60 { char pad0[0x1d72]; unsigned short flags; };
struct Foo0207df50 { char data[0x68]; };
struct Obj02049f50;
struct InitStruct02078484Struct {
    char pad0[0x10]; unsigned char mode;
    unsigned char enabled : 1; unsigned char flags : 7;
    short field12; short memberId; char pad16[0x16];
    Vector3fix position; char pad38[0xc]; Vector3fix scale;
};
struct TaskKinds { unsigned char values[3]; };
struct BattleConstants { char pad0[10]; TaskKinds kinds; };
struct BattleWork {
    char pad0[0x30]; SafeAllocator allocator;
    char pad44[0x258]; BattleState* battle; PartyState* party;
    char pad2a4[0x34bc]; Container_021dae60 slots;
    char pad54d4[0x2278]; SafeAllocator effectAllocator;
    Foo0207df50 graphics;
};
struct WorkTail {
    char pad0[0x7711]; unsigned char active; signed char animationState; char pad7713;
    short archiveTask; short memberObjects[4]; short effectTask1; short effectTask2;
    short effectObject1; short effectObject2; short timer;
    char pad7728[0x21]; unsigned char changed;
};
void CopyInternalFields0207df50(Foo0207df50*);
void SetBrightness(GameResources*, int, int);
void SetMainBrightness(GameResources*, int, int);
int IsBrightnessTransitionActive(GameResources*);
void RunFlaglearAndSetMode02167e6c_02167e6c(unsigned char*);
void SetForwardAndStore0205eb54(void*, int, int);
void DispatchIfField0xc4NonNeg_0205eb90(void*, int, int);
void SetBitsInField4(unsigned int*, unsigned int);
void ClearBitsInField4(unsigned int*, unsigned int);
void InitStruct02078484(InitStruct02078484Struct*);
int FindNodeAndProcess02057fb4(void*, int, InitStruct02078484Struct*);
void ForwardField0xc0_0205eb80(void*);
void SetField41AndScaleSub02049f50(Obj02049f50*, int, int);
void ResetTwoFields021637c8(void*);
void UpdateFlag8AndBit4_021636ac(void*);
void ClearFlagIfParityMismatch02163710_02163710(void*, int);
extern "C" {
int func_ov000_021759f4(Container_021dae60*);
void* func_02057924();
void func_02057e6c(void*, unsigned char, SafeAllocator*, void*, unsigned int, Foo0207df50*);
void func_ov000_021626a0(BattleWork*, int, int);
void func_02057f00(void*, int);
void func_ov000_02175808(Container_021dae60*);
void func_ov026_021dc440(SafeAllocator*, BattleWork*, BattleState*, PartyState*, const char*);
void func_ov000_02163894(BattleWork*);
void func_ov000_02163928(BattleWork*);
int func_ov026_021dc040(SafeAllocator*, BattleWork*, BattleState*, PartyState*);
}
extern char data_02108760[];
extern const char data_ov026_021dee0c[];
extern const char data_ov026_021dee23[];
extern const char data_ov026_021dee3a[];
extern const char data_ov026_021dedec[];
extern const BattleConstants data_ov026_021de6a0;

// USA: func_ov026_021db5a4
extern "C" ARM void func_ov026_021db5a4(BattleWork* self) {
    GameResources* resources = func_ov017_0218b5b0();
    WorkTail* tail = (WorkTail*)self;
    if (tail->animationState == 0) {
        if (func_ov000_021759f4(&self->slots)) {
            self->effectAllocator.Reset();
            CopyInternalFields0207df50(&self->graphics);
            tail->archiveTask = tail->effectTask1 = tail->effectTask2 = -1;
            SetBrightness(resources, -16, 15);
            tail->archiveTask = BackgroundLoader::GetInstance()->QueueLoadFile(data_ov026_021dee0c, &self->effectAllocator);
            tail->effectTask1 = BackgroundLoader::GetInstance()->QueueLoadFile(data_ov026_021dee23, &self->effectAllocator);
            tail->effectTask2 = BackgroundLoader::GetInstance()->QueueLoadFile(data_ov026_021dee3a, &self->effectAllocator);
            tail->animationState = 1;
            tail->changed = 1;
        }
    } else if (tail->animationState == 1) {
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        if (!loader->GetTaskStatus(tail->effectTask1) || !loader->GetTaskStatus(tail->effectTask2) || !loader->GetTaskStatus(tail->archiveTask) || IsBrightnessTransitionActive(resources)) return;
        short tasks[3];
        tasks[0] = tail->archiveTask;
        tasks[1] = tail->effectTask1;
        tasks[2] = tail->effectTask2;
        TaskKinds kinds = data_ov026_021de6a0.kinds;
        int index;
        short* task = tasks;
        unsigned char* kind = kinds.values;
        for (index = 0; index < 3; ++index) {
            void* data;
            unsigned int length;
            loader->GetLoadedFileByID(*task, &data, &length);
            func_02057e6c(func_02057924(), *kind++, &self->effectAllocator, data, length, &self->graphics);
            loader->RemoveTask(*task++);
        }
        tail->archiveTask = tail->effectTask1 = tail->effectTask2 = -1;
        func_ov000_021626a0(self, 0x26, 1);
        func_ov000_021626a0(self, 0x27, 0);
        RunFlaglearAndSetMode02167e6c_02167e6c((unsigned char*)self);
        tail->animationState = 2;
    } else if (tail->animationState == 2) {
        SetForwardAndStore0205eb54(data_02108760, 0x1b3, 0x1b3);
        DispatchIfField0xc4NonNeg_0205eb90(data_02108760, 1, 0);
        tail->active = 1;
        func_ov000_021626a0(self, 0x26, 0);
        func_ov000_021626a0(self, 0x27, 0);
        SetBitsInField4((unsigned int*)resources, 0x100000);
        InitStruct02078484Struct params;
        InitStruct02078484(&params);
        params.enabled = 1;
        params.mode = 1;
        params.scale.x = 0x800;
        params.scale.y = 0x800;
        params.scale.z = 0x800;
        params.position.x = 0;
        params.position.z = 0;
        params.position.y = -0xa000;
        tail->effectObject1 = FindNodeAndProcess02057fb4(func_02057924(), 0x1d, &params);
        tail->effectObject2 = FindNodeAndProcess02057fb4(func_02057924(), 0x1e, &params);
        tail->timer = 0;
        SetMainBrightness(resources, 0, 15);
        tail->animationState = 3;
    } else if (tail->animationState == 3) {
        int complete = 0;
        GameObject* effect = GameState::GetInstance()->GetGameObjectByIndex(tail->effectObject1);
        if (effect) {
            int delta = effect->obj3D_.normalizedAnimationTime_ - effect->obj3D_.prevFrameNormalizedAnimationTime_;
            if (delta > 0) {
                int step = fix32_Divide(effect->obj3D_.animationTime_ - effect->obj3D_.prevFrameAnimationTime_, delta);
                if (FIX32_MULTIPLY(step, 0x1000 - effect->obj3D_.normalizedAnimationTime_) <= 0x1e000) complete = 1;
            }
        }
        if (!effect || complete) {
            tail->timer = 0;
            SetMainBrightness(resources, -16, 15);
            tail->animationState = 4;
        }
    } else if (tail->animationState == 4) {
        if (!IsBrightnessTransitionActive(resources)) {
            SetMainBrightness(resources, -16, 0);
            func_02057f00(func_02057924(), 0x1d);
            func_02057f00(func_02057924(), 0x1e);
            tail->effectObject1 = tail->effectObject2 = -1;
            func_ov000_021626a0(self, 0x26, 1);
            ClearBitsInField4((unsigned int*)resources, 0x100000);
            tail->animationState = 5;
        }
    } else if (tail->animationState == 5) {
        ForwardField0xc0_0205eb80(data_02108760);
        tail->active = 0;
        SetMainBrightness(resources, -16, 0);
        GameState* game = GameState::GetInstance();
        InitStruct02078484Struct params;
        InitStruct02078484(&params);
        for (int index = 0; index < 4; ++index) {
            GameObject* member = game->GetCombatantByIndex(index);
            if (member) {
                SetField41AndScaleSub02049f50((Obj02049f50*)member, 31, 0);
            }
            params.memberId = index;
            params.mode = 0;
            tail->memberObjects[index] = FindNodeAndProcess02057fb4(func_02057924(), 0x12, &params);
        }
        func_ov000_02175808(&self->slots);
        self->slots.flags |= 0x200;
        ResetTwoFields021637c8(self);
        func_ov026_021dc440(&self->effectAllocator, self, self->battle, self->party, data_ov026_021dedec);
        func_ov000_02163894(self);
        UpdateFlag8AndBit4_021636ac(self);
        tail->animationState = 6;
        SetForwardAndStore0205eb54(data_02108760, 0x1b3, 0x1b3);
        DispatchIfField0xc4NonNeg_0205eb90(data_02108760, 2, 0);
        tail->active = 1;
    } else if (tail->animationState == 6) {
        func_ov000_02163928(self);
        if (func_ov026_021dc040(&self->allocator, self, self->battle, self->party)) {
            ClearFlagIfParityMismatch02163710_02163710(self, 0);
            tail->animationState = -1;
        }
    } else if (tail->animationState == -1) {
        func_ov000_02163928(self);
    }
}
