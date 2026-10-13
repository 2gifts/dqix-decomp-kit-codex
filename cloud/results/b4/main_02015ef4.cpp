#include <globaldefs.h>
#include <GameState/GameState.h>
#include <World/LootableContainer.h>
#include <Filesystem/BackgroundLoader.h>
#include <Resource/GameResources.h>

struct Foo02048004;
struct StructAt020473c8;
struct GetElement02094d5cList;
struct Holder020159c0;
struct Container020dedd0;
struct StructDE234_020de234;
struct HeadNode02046b24;
struct State0207dfc8;
struct Vec3copy0202ec84;

struct ChestRecord {
    unsigned short id;
    short key;
    unsigned short unk4 : 2;
    unsigned short lootType : 2;
    unsigned short rest : 12;
};

struct PackedElement {
    char pad0[0x10];
    unsigned int low : 20;
    unsigned int code : 8;
    unsigned int high : 4;
    short pad14;
    short pad16;
    short marker;
};

struct LoadEntry {
    char pad0[0xe];
    unsigned short scale;
    unsigned short counter;
    short taskID;
    signed char owner;
    char pad15;
    unsigned char state;
    unsigned char kind;
    unsigned char special;
    unsigned char delay;
    char pad1a[2];
    unsigned short uniqueID;
    char pad1e[2];
    void* object;
};

struct ObjectRecord {
    char pad0[0x1c];
    int posX;
    int posY;
    int posZ;
    char pad28[0x82 - 0x28];
    unsigned short timer;
};

struct ObjectSlot {
    char pad0[0x84];
    unsigned char flags;
    char pad85[3];
};

struct RootFlags {
    char pad[3];
    unsigned char hidden;
};

struct LoadManager {
    char pad0[0x50];
    State0207dfc8* stateSrc;
    char pad54[0x68 - 0x54];
    int param68;
    char pad6c[0x47c - 0x6c];
    LoadEntry* entries;
    char pad480[0x600 - 0x480];
    ObjectSlot slots[1];
    char pad688[0x820 - 0x688];
    int timer;
    char pad824[0x23bd - 0x824];
    unsigned char dirty;
    char pad23be[0x2724 - 0x23be];
    char elements[0x30];
    char keyed[1];
};

void MaybeInvoke0204719c(Foo02048004*);
extern "C" int func_02015ae4(LoadEntry*);
extern "C" int func_02015cd0(LoadEntry*);
void* GetElement02094d5c(GetElement02094d5cList*, int);
void ClearMatchingEntries020159c0(Holder020159c0*, int);
void* FindElementByKey020dedd0(Container020dedd0*, int);
int GetPreferredPackedField020de234(StructDE234_020de234*, int);
extern "C" int sprintf(char*, const char*, ...);
int GetHeadNodeIdOrMinusOne(HeadNode02046b24**);
extern "C" void func_0204719c(void*);
void CopyState0207dfc8(State0207dfc8*, State0207dfc8*);
void RestorePairTables0207df90(char*);
void Forward02047b30(void*, int, int, int);
void BackupPairTables0207dfac(char*);
void* GetField0x3b0Value(GameState*);
void ComputeTwoFromVec3_0202ec84(void*, Vec3copy0202ec84*, int*, int*);
void RenderFlaggedIndexedEntry(StructAt020473c8*, int);

struct NameBuf {
    char s[0x20];
};
extern NameBuf data_020e6e88;
extern char data_020ef20b[];

// USA: func_02015ef4
extern "C" ARM int func_02015ef4(LoadManager* self, int index) {
    GameState* gs = GameState::GetInstance();
    LoadEntry* entry = &self->entries[index];
    LootableContainerManager* containers = LootableContainerManager::GetMainInstance();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    int protagonistId = gs->GetProtagonist()->obj3D_.unknown_4_;
    int playerId = gs->GetUnknownGameObject()->obj3D_.unknown_4_;
    int visible = 0;
    self->dirty = 0;
    if (entry->state == 0 || entry->state == 8) {
        if (entry->object) {
            MaybeInvoke0204719c((Foo02048004*)entry->object);
            entry->object = 0;
        }
        return -1;
    }
    unsigned int delta = gs->GetEffectiveDeltaTime();
    int tick = gs->GetTickCount() + 1;
    if (entry->state == 2) {
        self->dirty = 1;
        func_02015ae4(entry);
        ChestRecord* rec = 0;
        if (entry->kind == 0 || entry->kind == 1) {
            rec = (ChestRecord*)containers->GetContainerByID(entry->uniqueID, 0);
        } else if (entry->kind == 2) {
            rec = (ChestRecord*)GetElement02094d5c((GetElement02094d5cList*)self->elements, entry->uniqueID);
        }
        if (!rec) {
            entry->state = 8;
            return 0;
        }
        ClearMatchingEntries020159c0((Holder020159c0*)self, (signed char)protagonistId);
        if (entry->owner == playerId) {
            if (rec->lootType == 1) {
                NameBuf name = data_020e6e88;
                entry->taskID = loader->QueueLoadFile(name.s, 0);
                entry->state = 4;
                return 0;
            } else if (rec->lootType == 2) {
                PackedElement* packed = (PackedElement*)FindElementByKey020dedd0((Container020dedd0*)self->keyed, rec->key);
                if (packed && packed->marker != 0x5617) {
                    int field = GetPreferredPackedField020de234((StructDE234_020de234*)packed, 0);
                    char path[0x50];
                    sprintf(path, data_020ef20b, (signed char)packed->code, field);
                    entry->taskID = loader->QueueLoadFile(path, 0);
                    entry->state = 4;
                    return 0;
                }
            }
        }
        entry->object = 0;
        entry->state = 5;
    } else if (entry->state == 4) {
        self->dirty = 1;
        func_02015ae4(entry);
        if (loader->GetTaskStatus(entry->taskID) == 0) return 0;
        void* file = 0;
        unsigned int size = 0;
        loader->GetLoadedFileByID(entry->taskID, &file, &size);
        GameResources* root = func_ov017_0218b5b0();
        HeadNode02046b24** head = (HeadNode02046b24**)root->unknown_ptr_array_36fc[0];
        if (file && GetHeadNodeIdOrMinusOne(head) != 4) {
            ObjectSlot* slot = &self->slots[entry->owner];
            entry->object = slot;
            func_0204719c(slot);
            ((ObjectSlot*)entry->object)->flags |= 4;
            char saved[0x70];
            CopyState0207dfc8(self->stateSrc, (State0207dfc8*)saved);
            RestorePairTables0207df90(saved);
            Forward02047b30(entry->object, (int)file, size, self->param68);
            BackupPairTables0207dfac(saved);
            self->timer = 0x1f;
            entry->delay = 0;
        } else {
            entry->object = 0;
        }
        loader->RemoveTask(entry->taskID);
        entry->taskID = -1;
        entry->state = 5;
    } else if (entry->state == 5) {
        self->dirty = 1;
        if (entry->counter > 0xd) visible = 1;
        if (func_02015ae4(entry)) {
            entry->counter = 0;
            entry->state = 6;
        }
    } else if (entry->state == 6) {
        self->dirty = 1;
        visible = 1;
        entry->counter += delta;
        if (entry->counter > 500) {
            entry->state = 7;
            entry->counter = 0;
        }
    } else if (entry->state == 7) {
        self->dirty = 1;
        visible = 1;
        if (entry->special == 1) {
            if (func_02015cd0(entry)) {
                entry->scale = 0x647a;
                entry->state = 1;
                if (entry->object) {
                    MaybeInvoke0204719c((Foo02048004*)entry->object);
                    entry->object = 0;
                }
            }
        } else if (func_02015cd0(entry)) {
            entry->scale = 0x4000;
            entry->state = 8;
            if (entry->object) {
                MaybeInvoke0204719c((Foo02048004*)entry->object);
                entry->object = 0;
            }
        }
    }
    if (((RootFlags*)func_ov017_0218b5b0()->unknown_ptr_array_3afc[0x22])->hidden != 0) visible = 0;
    if (entry->object && visible) {
        self->dirty = 1;
        int x = 0;
        int y = 0;
        void* camera = GetField0x3b0Value(gs);
        if (camera) ComputeTwoFromVec3_0202ec84(camera, (Vec3copy0202ec84*)entry, &x, &y);
        x -= 12;
        y -= 0x28;
        if (entry->delay < 10) entry->delay++;
        if (entry->state == 7 && entry->object) {
            if (tick < self->timer) {
                self->timer -= tick;
            } else {
                self->timer = 0;
                entry->object = 0;
                return 0;
            }
        }
        if (x >= -24 && x < 256 && y >= -24 && y < 192) {
            int timer = self->timer;
            ((ObjectRecord*)entry->object)->timer = timer;
            int px = x;
            int offsetY = y - entry->delay;
            ObjectRecord* obj = (ObjectRecord*)entry->object;
            obj->posX = px << 12;
            obj->posY = offsetY << 12;
            obj->posZ = 0;
            RenderFlaggedIndexedEntry((StructAt020473c8*)entry->object, 1);
        }
    }
    return 0;
}
