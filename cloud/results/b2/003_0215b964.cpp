#include <globaldefs.h>
#include "std_library_functions.h"

struct GameState {
    static GameState* GetInstance();
};

struct Container020e0310;

struct CombatantInfo {
    char name[0x30];
    unsigned short maxHp;
};

struct CombatantStats {
    unsigned int flags;
    unsigned short hp;
};

struct Combatant {
    char pad0[4];
    short id;
    char pad6[0x130 - 6];
    CombatantStats* stats;
    CombatantInfo* info;
};

struct TextBuffer {
    char pad0[0x8e0];
    char dst[0x40];
    char src[0x40];
};

struct TextWriter {
    void* args;
};

struct StoreStruct;

TextWriter* GetGlobalField0x1c020421a0();
int GetFieldByKey020e0434(Container020e0310* c, int key);
int AppendFormatted02041fac(char* buf, int a, int b);
int AppendFrameTag02041c08(char* dst, int a1, int a2, int a3, int a4, int a5);
int AppendCursorTag(char* dst, int cursor);
Combatant* GetCombatantWithFlag0x100(GameState* gs, int index);
unsigned char* GetFieldAt0x150(unsigned char* obj);
int AppendPaletteTag(char* dst, int palette);
int AppendXYTag(char* dst, int x, int y);
int AppendString02042058(char* dst, const char* src);
int AppendNameTag(char* dst, int n, const char* name);
int AppendXTag(char* dst, int x);
void InitObjFromCombatantId020e4bf4(void* obj, int id);
extern "C" void func_02046380(void* g);
extern "C" void func_02046608(void* g, int a, char* src, char* dst, int cap, int e, int f);
void StoreInArray0x8b0(StoreStruct* base, int index, int value);
void SetByteInRange(unsigned char* base, int index, unsigned char value);
void SetByteAtIndex(unsigned char* base, int index, unsigned char value);
int CallFunc020e0434With02153694(int value);

// USA: func_ov003_0215b964
extern "C" ARM void func_ov003_0215b964(unsigned char* self, char* out, int flag) {
    if (out == 0) return;
    TextBuffer* buf;
    TextWriter* writer = GetGlobalField0x1c020421a0();
    signed char cursor = (signed char)self[0x585];
    buf = *(TextBuffer**)(self + 0x7c);
    Container020e0310* strings = (Container020e0310*)(self + 0x64);
    if (cursor == -1) {
        AppendFormatted02041fac(out, GetFieldByKey020e0434(strings, 10), 0xe);
    } else {
        AppendFormatted02041fac(out, GetFieldByKey020e0434(strings, 11), 0xe);
        if (flag) {
            AppendFrameTag02041c08(out, cursor, 8, 5, 5, 5);
        }
        AppendCursorTag(out, cursor);
    }
    GameState* gs = GameState::GetInstance();
    for (int i = 0; i < self[0x590]; i++) {
        Combatant* c = GetCombatantWithFlag0x100(gs, (self + i)[0x591]);
        unsigned char* field = GetFieldAt0x150((unsigned char*)c);
        CombatantStats* stats = c->stats;
        unsigned int flags = stats->flags;
        unsigned short maxHp = c->info->maxHp;
        float ratio;
        if ((float)maxHp == 0.0f) {
            ratio = 0.0f;
        } else {
            ratio = (float)stats->hp / (float)maxHp;
        }
        if (flags & 1) {
            AppendPaletteTag(out, 9);
        } else if (ratio <= 0.08f) {
            AppendPaletteTag(out, 0xb);
        } else if (ratio <= 0.25f) {
            AppendPaletteTag(out, 0xd);
        } else {
            AppendPaletteTag(out, 0xf);
        }
        AppendXYTag(out, 0xc, i * 14 + 0x14);
        char* name = c->info->name;
        if (cursor == -1) {
            AppendString02042058(out, name);
        } else {
            AppendNameTag(out, i, name);
        }
        memset(buf->dst, 0, 0x40);
        memset(buf->src, 0, 0x40);
        AppendXTag(buf->src, 0x40);
        if (c->stats->flags & 1) {
            AppendString02042058(buf->src, (const char*)GetFieldByKey020e0434(strings, 0xd));
        } else if (flags & 2) {
            AppendString02042058(buf->src, (const char*)GetFieldByKey020e0434(strings, 0xe));
        } else if (flags & 4) {
            AppendString02042058(buf->src, (const char*)GetFieldByKey020e0434(strings, 0xf));
        }
        char obj[12];
        InitObjFromCombatantId020e4bf4(obj, c->id);
        func_02046380(writer);
        writer->args = obj;
        func_02046608(writer, 0xa, buf->src, buf->dst, 0x100, 0, 0);
        AppendString02042058(out, buf->dst);
        if (c->stats->flags & 1) continue;
        if (flags & 6) continue;
        func_02046380(writer);
        StoreInArray0x8b0((StoreStruct*)writer, 0, *(unsigned short*)(field + *(int*)(field + 0x950) * 2 + 0x16c));
        SetByteInRange((unsigned char*)writer, 0, 2);
        SetByteAtIndex((unsigned char*)writer, 0, 1);
        memset(buf->dst, 0, 0x40);
        memset(buf->src, 0, 0x40);
        AppendXTag(buf->src, 0x40);
        AppendString02042058(buf->src, (const char*)CallFunc020e0434With02153694(1011));
        AppendString02042058(buf->src, (const char*)GetFieldByKey020e0434(strings, 0xc));
        func_02046608(writer, 0xa, buf->src, buf->dst, 0x100, 0, 0);
        AppendString02042058(out, buf->dst);
    }
}
