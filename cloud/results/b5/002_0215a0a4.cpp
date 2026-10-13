#include <globaldefs.h>
#include "GameState/GameState.h"

struct Container020e0310;
struct Obj02046574 { void* first; void* second; void* third; void* fourth; void* fifth; };
struct Obj02048350 {
    unsigned char pad0[4];
    short id;
    unsigned char pad6[0x12a];
    unsigned int* flags;
    char* name;
};
struct SearchEntry { char* name; int pad4; unsigned int count : 8; };
struct BattleEntry { unsigned char kind; unsigned char pad[5]; };
struct BattleArgs {
    unsigned char pad0[2];
    signed char who;
    unsigned char count;
    signed char ids[4];
    BattleEntry entries[4];
};
struct BattleCtx {
    unsigned char pad0[8];
    unsigned char objA[12];
    unsigned char objB[12];
    unsigned char strings[0x1b98];
    int mode;
    int sub;
    int extra;
    unsigned char pad1bc4[0xc];
    char* buf;
    char* backup;
    unsigned char pad1bd8[0x48];
    signed char leader;
    signed char partyIdx;
    unsigned char pad1c22[4];
    short msgId;
    short msgA;
    short msgB;
    unsigned char pad1c2c[5];
    unsigned char dirty;
    unsigned char pad1c32[0x856];
    short altMsg;
    unsigned char active;
};

void* GetData02153637();
void Dispatch020e3428(void*, int);
Obj02046574* GetGlobalField0x1c020421a0();
extern "C" int func_0202ae18();
char* GetData02108e10();
SearchEntry* SearchBothTables02079e2c(char*, int);
extern "C" void* memset(void*, int, unsigned int);
extern "C" void* memcpy(void*, const void*, unsigned int);
extern "C" void func_02046380(Obj02046574*);
void Clear12Bytes020e46c4(void*);
void SetIndexedName02046574(Obj02046574*, int, char*);
void InitObjFromCombatantId020e4bf4(void*, int);
void ConsumeCounter02048350(Obj02048350*, int);
int CheckField0NonZero(int*);
extern "C" void func_ov017_021c9e00(int, int, int, int);
char* GetFieldByKey020e0434(Container020e0310*, int);
extern "C" void func_02046608(Obj02046574*, int, char*, char*, int, int, int);
char* AppendString02042058(char*, const char*);
struct StoreStruct;
void StoreInArray0x8b0(StoreStruct*, int, int);
int GetByteField0x5721(GameState*);

// USA: func_ov002_0215a0a4
extern "C" ARM void func_ov002_0215a0a4(BattleCtx* self, BattleArgs* args, int flag) {
    char* buf;
    Dispatch020e3428(GetData02153637(), 1);
    GameState* gs = GameState::GetInstance();
    Obj02046574* names = GetGlobalField0x1c020421a0();
    int cmdObj = func_0202ae18();
    SearchEntry* entry = SearchBothTables02079e2c(GetData02108e10(), self->msgId);
    if (entry == 0) return;
    memset(self->buf, 0, 0x960);
    memset(self->backup, 0, 0x960);
    func_02046380(names);
    buf = self->buf;
    Clear12Bytes020e46c4(self->objA);
    Clear12Bytes020e46c4(self->objB);
    Obj02048350* actor = (Obj02048350*)GetCombatantWithFlag0x100(gs, args->who);
    if (actor != 0) {
        SetIndexedName02046574(names, 0, actor->name);
        InitObjFromCombatantId020e4bf4(self->objA, args->who);
        names->first = self->objA;
    }
    SetIndexedName02046574(names, 1, entry->name);
    if (self->msgId == 0x1e || self->msgId == 0x1f || self->msgId == 0x20 || self->msgId == 0x21 || self->msgId == 0x23 || self->msgId == 0x26 || self->msgId == 0x27) {
        int done;
        self->active = 1;
        self->msgA = 0x232d;
        self->msgB = -1;
        GameObject* member = gs->GetPartyMemberByIndex(self->partyIdx);
        done = 0;
        if (flag != 0 && member != 0) {
            if (self->msgId == 0x1e || self->msgId == 0x1f || self->msgId == 0x20 || self->msgId == 0x21) {
                self->altMsg = 0x232c;
                done = 1;
            } else if (self->msgId == 0x23) {
                self->altMsg = 0x794b;
                done = 1;
            } else if (self->msgId == 0x27 || self->msgId == 0x26) {
                self->altMsg = 0x2336;
                done = 1;
            }
        } else if (member == 0) {
            self->msgA = 0x232d;
            self->altMsg = 0x232b;
        } else if ((*((Obj02048350*)member)->flags & 1) != 0) {
            if (self->msgId == 0x26) {
                self->altMsg = 0x2337;
                done = 1;
            } else {
                self->altMsg = 0x232b;
            }
        } else if (args->entries[0].kind == 1) {
            self->msgA = 0x232d;
            self->altMsg = 0x232b;
        } else {
            self->msgA = 0x2333;
            self->altMsg = 0x2334;
        }
        self->mode = 0x26;
        self->sub = 0x11;
        self->extra = 0;
        if (done == 0) return;
        ConsumeCounter02048350(actor, entry->count);
        if (CheckField0NonZero((int*)cmdObj) != 0) {
            func_ov017_021c9e00(self->leader, 0, 0, 1);
        }
        return;
    }
    if (self->msgId != 0x22 && self->msgId != 0x310) return;
    if (flag != 0) {
        func_02046608(names, 0xc, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 0x232d), buf + 0x860, 0xe3, 0, 1);
        AppendString02042058(self->buf, buf + 0x860);
        unsigned char* kinds = &args->entries[0].kind;
        Obj02048350* other;
        for (int i = 0; i < args->count; i++) {
            int key = kinds[i * 6] == 3 ? 0x232c : 0x233a;
            if (key < 0) continue;
            other = (Obj02048350*)GetCombatantWithFlag0x100(gs, args->ids[i]);
            if (other == 0) continue;
            func_02046380(names);
            char* otherName = other->name;
            SetIndexedName02046574(names, 2, otherName);
            InitObjFromCombatantId020e4bf4(self->objB, args->ids[i]);
            names->fifth = self->objB;
            StoreInArray0x8b0((StoreStruct*)names, 9, other->id);
            func_02046608(names, 0xc, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), key), buf + 0x860, 0xe3, 0, 1);
            AppendString02042058(self->buf, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 8));
            AppendString02042058(self->buf, buf + 0x860);
        }
        ConsumeCounter02048350(actor, entry->count);
        if (CheckField0NonZero((int*)cmdObj) != 0) {
            func_ov017_021c9e00(self->leader, 0, 0, 1);
        }
    } else {
        Obj02048350* extra = (Obj02048350*)gs->GetUnknownGameObject();
        if (extra != 0) {
            SetIndexedName02046574(names, 2, extra->name);
            InitObjFromCombatantId020e4bf4(self->objB, extra->id);
            names->fifth = self->objB;
        }
        {
            AppendString02042058(buf + 0x860, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 0x2333));
            AppendString02042058(buf + 0x860, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 8));
            if (GetByteField0x5721(gs) == 1) {
                AppendString02042058(buf + 0x860, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 0x2334));
            } else {
                AppendString02042058(buf + 0x860, GetFieldByKey020e0434((Container020e0310*)((char*)self + 0x20), 0x2335));
            }
        }
        func_02046608(names, 0xc, buf + 0x860, self->buf, 0xe3, 0, 1);
    }
    self->dirty = 1;
    memcpy(self->backup, self->buf, 0x960);
    self->altMsg = -1;
    self->active = 0;
}
