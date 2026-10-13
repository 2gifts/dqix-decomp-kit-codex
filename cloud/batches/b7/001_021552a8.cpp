#include <globaldefs.h>
#include <GameState/GameState.h>
#include <Filesystem/BackgroundLoader.h>
#include <Memory/SafeAllocator.h>
#include <std_library_functions.h>

struct Obj0208b8c4 { char data[0xe4]; };
struct Struct0205a198 { char data[0x28]; };
struct ClearTarget0205a234 { int data[2]; };
struct ResourceTable {
    char pad0[0x3c]; ClearTarget0205a234* list; Struct0205a198* entries;
    char pad1[8]; short count; char pad2[2]; unsigned char flags; char pad3[3];
    void SetEntries(Struct0205a198* values, short capacity) { entries = values; count = capacity; }
};
struct ResourceDescriptor {
    int field_0; char pad0[3]; unsigned char hideObject;
    char pad1[0x40 - 8]; unsigned short identifier;
    char pad2[2]; unsigned short flags;
    char pad3[0x68 - 0x46];
};
struct LoadState {
    unsigned char field_0; unsigned char failed; char pad0[8]; unsigned short state;
    ResourceDescriptor descriptor;
    int mainTask; int scriptTask; SafeAllocator allocator;
    char pad1[4]; int ownerId;
    char pad2[4]; unsigned char effectMode;
    char pad3[0x107 - 0x9d]; unsigned char partyFlag;
    char pad4[8]; int graphicsTask;
    char pad5[0x120 - 0x114]; Obj0208b8c4* runtime; SafeAllocator runtimeAllocator;
    char pad6[0x168 - 0x138]; ResourceTable* table; ClearTarget0205a234* list; Struct0205a198* entries;
};
struct Foo0207df50 { char data[0x68]; };
struct StageState { char pad0[0xbd0]; Foo0207df50 graphics; };
struct SetFlagStruct;
struct ResourceView {
    char pad0[0xc4]; int effectState;
    char pad1[0x2cc - 0xc8]; StageState stage;
    char pad2[0x36d0 - 0xf04]; SetFlagStruct* field_36d0;
};
struct MessageWork { char pad0[0x8c]; Obj0208b8c4* runtime; };
struct PartyFlags { unsigned int flags; };
struct PartyMemberView { char pad0[0x130]; PartyFlags* party; };
struct ActorView { char pad0[0xb2]; unsigned short field_b2; };
struct BattleEffectWork {
    char pad0[0x45b]; unsigned char active[32];
    char pad1[0x490 - 0x47b]; signed char selected;
};
struct EffectEntry { unsigned short field_0; unsigned short flags; char pad0[0x70 - 4]; };
struct EffectNode { char pad0[0x14]; int count; char pad1[0x44 - 0x18]; EffectEntry* entries; };
struct Obj020196fc;
struct Pool020401fc;
struct ActiveEntry02046900;
struct Rec020467f0;
void* GetFieldIfFlag4(char*);
void TailForward02012da4(AllocatorUnion*, void*);
MessageWork* GetGlobalField0x1c020421a0();
void InitAllocatorsAndBuffers0208b8c4(Obj0208b8c4*, SafeAllocator*);
Pool020401fc* GetGlobalPtr021075f4();
void EmptyStub0203d004();
void InitPool020401fc(Pool020401fc*, unsigned int, SafeAllocator*);
void Init0205a198(Struct0205a198*);
void ClearField0And40205a234(ClearTarget0205a234*);
void InitStruct0205a444(char*);
int CountActiveEntries(ActiveEntry02046900*);
void* FindRecordByIndex(Rec020467f0*, int, void**, int*);
void InitOrClearAllocState_021538d4(int, void*, SafeAllocator*);
void ClearAllocSlotsAndStore_0215aa14(SafeAllocator*);
void SetArrVal0215aaa8(int, int);
void ForwardArgsAndClearFlag8c_0215acf8(int, int);
void CopyInternalFields0207df50(Foo0207df50*);
int StoreArgIfMatched_02153948(unsigned short);
EffectNode* GetNodeAtDepth020196fc(Obj020196fc*, int);
void SetOrClearBitInArray(void*, unsigned char*, int, int);
void* GetDataPtr02114e04_020d6c00();
void ProcessBattlerEffects02019568(void*, int);
Object3D* GetField4334_021bdbc0(unsigned char*);
void SetField0x238False(void*);
void ClearIntAt0x23c(unsigned char*);
void SetFlag0x9c6(SetFlagStruct*, int);
extern "C" {
void func_0209c2e0(void*, int, int);
void func_ov017_021bb27c(LoadState*);
void func_0208b9a0(Obj0208b8c4*, int);
void func_0205a528(ResourceTable*, void*, int, SafeAllocator*);
void func_ov001_0215a850(SafeAllocator*);
void func_ov001_0215a8dc();
BattleEffectWork* func_0205ec34();
Obj020196fc* func_02012fe4();
void* func_0202ae18();
}
extern AllocatorUnion data_02114e20;
extern char data_02109bf4;

// USA: func_ov001_021552a8
extern "C" ARM int func_ov001_021552a8(LoadState* self) {
    StageState* stage;
    GameState* game = GameState::GetInstance();
    ResourceView* resources = (ResourceView*)func_ov017_0218b5b0();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    void* controller = GetFieldIfFlag4((char*)game);
    GameObject* actor = game->GetUnknownGameObject();
    stage = &resources->stage;
    if (!loader->GetTaskStatus(self->mainTask)) return self->state;
    if (!loader->GetTaskStatus(self->scriptTask)) return self->state;
    if ((self->descriptor.flags & 0x200) && !loader->GetTaskStatus(self->graphicsTask)) return self->state;
    if (loader->GetDetailedTaskStatus(self->mainTask) != 2 || loader->GetDetailedTaskStatus(self->scriptTask) != 2) {
        SignedAllocatorHeader* allocation = self->allocator.GetSignedAllocator();
        self->allocator.Destroy();
        TailForward02012da4(&data_02114e20, allocation);
        loader->RemoveTask(self->mainTask);
        loader->RemoveTask(self->scriptTask);
        loader->RemoveTask(self->graphicsTask);
        func_0209c2e0(&data_02109bf4, self->ownerId, 0);
        func_ov017_021bb27c(self);
        self->failed = 1;
        return self->state;
    }
    void* mainData;
    void* scriptData;
    unsigned int mainSize;
    unsigned int scriptSize;
    loader->GetLoadedFileByID(self->mainTask, &mainData, &mainSize);
    loader->GetLoadedFileByID(self->scriptTask, &scriptData, &scriptSize);
    if (self->descriptor.flags & 2) {
        self->runtime = (Obj0208b8c4*)self->allocator.Allocate(sizeof(Obj0208b8c4));
        func_0208b9a0(self->runtime, 0);
        MessageWork* message = GetGlobalField0x1c020421a0();
        void* buffer = self->allocator.Allocate(0xa000);
        self->runtimeAllocator.CreateTypeA(buffer, 0xa000);
        self->runtimeAllocator.Reset();
        InitAllocatorsAndBuffers0208b8c4(self->runtime, &self->runtimeAllocator);
        message->runtime = self->runtime;
    }
    if (self->descriptor.flags & 1 || self->descriptor.flags & 4) {
        Pool020401fc* pool = GetGlobalPtr021075f4();
        EmptyStub0203d004();
        InitPool020401fc(pool, 12, &self->allocator);
    }
    if (self->descriptor.flags & 0x200) {
        self->allocator.GetMaxPossibleAllocation();
        self->entries = (Struct0205a198*)self->allocator.Allocate(0x230);
        self->table = (ResourceTable*)self->allocator.Allocate(sizeof(ResourceTable));
        self->list = (ClearTarget0205a234*)self->allocator.Allocate(sizeof(ClearTarget0205a234));
        for (int index = 0; index < 14; ++index) Init0205a198(&self->entries[index]);
        ClearField0And40205a234(self->list);
        InitStruct0205a444((char*)self->table);
        self->table->flags = 0;
        self->table->SetEntries(self->entries, 14);
        self->table->list = self->list;
        void* recordData;
        void* graphicsData;
        unsigned int graphicsSize;
        int recordSize;
        loader->GetLoadedFileByID(self->graphicsTask, &graphicsData, &graphicsSize);
        int count = CountActiveEntries((ActiveEntry02046900*)graphicsData);
        for (int index = 0; index < count; ++index) {
            void* record = FindRecordByIndex((Rec020467f0*)graphicsData, index, &recordData, &recordSize);
            func_0205a528(self->table, record, recordSize, &self->allocator);
        }
        loader->RemoveTask(self->graphicsTask);
        self->graphicsTask = -1;
    }
    func_ov001_0215a850(&self->allocator);
    func_ov001_0215a8dc();
    InitOrClearAllocState_021538d4((int)mainData, 0, &self->allocator);
    ClearAllocSlotsAndStore_0215aa14(&self->allocator);
    if (self->descriptor.flags & 4) SetArrVal0215aaa8(1, (int)&resources->effectState);
    ForwardArgsAndClearFlag8c_0215acf8((int)scriptData, scriptSize);
    GameObject* member = game->GetPartyMemberByIndex(0);
    if (member && (((PartyMemberView*)member)->party->flags & 1)) self->partyFlag = 1;
    loader->RemoveTask(self->mainTask);
    loader->RemoveTask(self->scriptTask);
    CopyInternalFields0207df50(&stage->graphics);
    if (!StoreArgIfMatched_02153948(self->descriptor.identifier)) {
        SignedAllocatorHeader* allocation = self->runtimeAllocator.GetSignedAllocator();
        if (allocation) {
            self->runtimeAllocator.Destroy();
            self->allocator.Free(allocation);
        }
        allocation = self->allocator.GetSignedAllocator();
        self->allocator.Destroy();
        TailForward02012da4(&data_02114e20, allocation);
        func_ov017_021bb27c(self);
        self->failed = 1;
        return self->state;
    }
    int index;
    int count;
    EffectNode* node;
    BattleEffectWork* effects = func_0205ec34();
    node = GetNodeAtDepth020196fc(func_02012fe4(), 0);
    count = node->count;
    if (node) {
        memset(effects->active, 0, sizeof(effects->active));
        for (index = 0; index < count; ++index) {
            EffectEntry* entry = &node->entries[index];
            if (entry && !(entry->flags & 4)) SetOrClearBitInArray(effects, effects->active, (unsigned char)index, 1);
        }
    }
    unsigned char effectMode = self->effectMode;
    ResourceDescriptor descriptor = self->descriptor;
    GameState* currentGame = GameState::GetInstance();
    GameResources* currentResources = func_ov017_0218b5b0();
    Obj020196fc* currentBattle = func_02012fe4();
    func_0202ae18();
    GetPtrField0x2a04(currentGame);
    GetDataPtr02114e04_020d6c00();
    if (!(descriptor.flags & 0x400)) ProcessBattlerEffects02019568(currentBattle, effectMode);
    func_0205ec34()->selected = -1;
    if (descriptor.hideObject) {
        Object3D* object = GetField4334_021bdbc0((unsigned char*)currentResources);
        if (object) object->MakeHidden();
    }
    SetField0x238False(controller);
    ClearIntAt0x23c((unsigned char*)controller);
    ((ActorView*)actor)->field_b2 = 0;
    SetFlag0x9c6(resources->field_36d0, 0);
    return 3;
}
