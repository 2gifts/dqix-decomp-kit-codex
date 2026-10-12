#include <globaldefs.h>
#include <GameState/GameState.h>

struct Struct02020520 {
    int pointer;
    short x;
    short y;
    short width;
    short height;
};

struct S_b20c {
    int pointer;
    int layout;
    unsigned char mode;
};

struct NameDescriptor {
    char* singular;
    char* plural;
    unsigned int flags;
};

struct MessageStore020200c0 {
    NameDescriptor* names;
    char pad4[0x60 - 0x4];
    void* tileBuffer;
};

struct PartyRecord020200c0 {
    char pad0[0x186];
    unsigned char revocations[0x950 - 0x186];
    int vocation;
};

struct PartyMember020200c0 {
    char pad0[0x130];
    unsigned int* status;
    char* name;
    char pad138[0x150 - 0x138];
    PartyRecord020200c0* record;
};

struct Obj0205b674;

extern "C" GameResources* func_ov017_0218b5b0(void);
MessageStore020200c0* GetGlobalField0x1c020421a0(void);
extern "C" void* memcpy(void* dst, const void* src, unsigned int n);
void ClearStruct02020520(struct Struct02020520* s);
extern "C" void func_0205b890(struct Struct02020520* layout, int mode, int value);
void ClearTwoWordsAndByte(struct S_b20c* obj);
void SetField0_0205b220(void* obj, int value);
void SetFieldsAt0x4And0x8_0205b228(void* obj, int value, unsigned char flag);
extern "C" void func_0205b734(struct S_b20c* dest, int x, int y, int width, int height, int mode);
const char* CallFunc020e0434With02153694(int key);
extern "C" void func_0205b234(struct S_b20c* dest, short x, int y, const char* text, int color, int mode);
extern "C" int func_020420e8(const char* text, int large);
extern "C" int func_ov017_021bdbcc(GameResources* res);
void* GetTableEntry020421b0(int index);
extern "C" void func_0205b530(struct S_b20c* dest, void* tile, short x, short y, int size, int color);
unsigned short GetTableValue(void* obj);
void PositionAndFormatValueLabel0205b674(struct Obj0205b674* obj, short posX, int param2, int param3,
                                         unsigned char param4, unsigned char param5, unsigned char param6, unsigned char param7);
extern "C" void __clear(void* buf, int n);
extern "C" int sprintf(char* dst, const char* fmt, ...);
void Clear12Bytes020e46c4(void* p);
void InitObjFromCombatantId020e4bf4(void* obj, int combatantId);
extern "C" void func_02046608(MessageStore020200c0* store, int a, const char* format, char* output, int size, int b, int c);
void DelayThenSyncBit0(void);
void CleanInvalidateCacheRange(const void* addr, unsigned int size);
extern "C" void LoadToSubObjVRAM(const void* src, unsigned int offset, unsigned int size);
extern char data_020ef580[];

// USA: func_020200c0
extern "C" ARM void func_020200c0(int combatantId, int value) {
    GameState* gameState = GameState::GetInstance();
    int isPartySlot;
    if (combatantId >= 0 && combatantId <= 3) isPartySlot = 1;
    else isPartySlot = 0;
    if (!isPartySlot || value == 0) return;
    GameResources* res = func_ov017_0218b5b0();
    if (res == NULL || res->unknown_ptr_36d0 == NULL) return;
    unsigned int offset = (combatantId + 1) << 11;
    MessageStore020200c0* store = GetGlobalField0x1c020421a0();
    void* buffer = store->tileBuffer;
    if (buffer == NULL) return;
    memcpy(buffer, (void*)0x06600000, 0x800);

    Struct02020520 layout;
    ClearStruct02020520(&layout);
    func_0205b890(&layout, 1, value);
    S_b20c dest;
    ClearTwoWordsAndByte(&dest);
    SetField0_0205b220(&dest, (int)buffer);
    SetFieldsAt0x4And0x8_0205b228(&dest, (int)&layout, 1);
    func_0205b734(&dest, 7, 0x10, 0x17, 0x18, 1);
    func_0205b234(&dest, 7, 0xe, CallFunc020e0434With02153694(0xc9), 0xf, 0);
    func_0205b734(&dest, 7, 0x1c, 0x17, 0x24, 1);
    func_0205b234(&dest, 7, 0x1a, CallFunc020e0434With02153694(0xca), 0xf, 0);

    PartyMember020200c0* member = (PartyMember020200c0*)GetCombatantWithFlag0x100(gameState, combatantId);
    if (member == NULL) return;
    char* name = member->name;
    func_0205b234(&dest, (short)((0x40 - func_020420e8(name, 0)) >> 1), 3, name, 0xf, 0);

    int color;
    int messageKey;
    short vocation;
    int col;
    int row;
    unsigned char revocations;
    PartyRecord020200c0* record = member->record;
    revocations = record->revocations[record->vocation];
    vocation = record->vocation;
    if (func_ov017_021bdbcc(res)) vocation = 0xd;
    if (vocation == 0) vocation = -1;
    if (vocation == 0xd) vocation = 0;
    color = 0xf;
    unsigned int status = *member->status;
    messageKey = -1;
    if (status & 1) {
        messageKey = 0xe;
        color = 8;
    } else if (status & 2) {
        messageKey = color;
        color = 3;
    } else if (status & 4) {
        messageKey = 0x10;
    } else {
        int revocationColor = 5;
        if (revocations == 10) revocationColor = 0xd;
        const char* levelLabel = CallFunc020e0434With02153694(0x3f3);
        int labelWidth = func_020420e8(levelLabel, 0);
        if (vocation >= 0) {
            char* tile = (char*)GetTableEntry020421b0((unsigned char)(vocation * 4 + 0x28));
            for (row = 0; row < 2; row++) {
                for (col = 0; col < 2; col++) {
                    func_0205b530(&dest, tile, col * 8 + 6, row * 8 + 0x29, 8, 0xf);
                    tile += 0x20;
                }
            }
        }
        func_0205b234(&dest, 0x20, 0x2b, levelLabel, 0xf, 0);
        PositionAndFormatValueLabel0205b674((Obj0205b674*)&dest, labelWidth + 0x1b, 0x2b, GetTableValue(member), 0xf, 2, 1, 0);
        if (revocations != 0) {
            char count[7];
            __clear(count, 7);
            sprintf(count, data_020ef580, revocations);
            func_0205b234(&dest, 0x13, 0x2b, count, revocationColor, 0);
        }
    }
    if (messageKey >= 0) {
        NameDescriptor names;
        Clear12Bytes020e46c4(&names);
        InitObjFromCombatantId020e4bf4(&names, combatantId);
        const char* format = CallFunc020e0434With02153694(messageKey);
        char text[0x80];
        __clear(text, 0x80);
        store->names = &names;
        func_02046608(store, 10, format, text, 0x100, 0, 0);
        func_0205b234(&dest, (short)((0x40 - func_020420e8(text, 0)) >> 1), 0x2b, text, color, 0);
    }
    DelayThenSyncBit0();
    CleanInvalidateCacheRange(buffer, 0x800);
    LoadToSubObjVRAM(buffer, offset, 0x800);
}
