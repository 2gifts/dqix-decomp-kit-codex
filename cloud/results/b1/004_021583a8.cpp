#include <globaldefs.h>
#include "std_library_functions.h"
#include "GameState/GameState.h"
#include "Resource/GameResources.h"

struct Obj020397cc;
struct Obj24_021b1ffc;
struct Obj24_021b2010;
struct Obj24_021b2024;
struct Obj24_021b2038;
struct Obj24_021b1fe8;
struct TableA68;
struct S_a059c;

struct Messages_021583a8 {
    char pad0[0x5c];
    char* buf;
    char pad60[0x938];
    int status;
    int pad99c;
    int mode;
};

struct Five_021583a8 {
    unsigned char b0;
    unsigned char done;
};

struct BattleBlock_021583a8 {
    char data[0xb0];
};

struct Blk_021583a8 {
    char pad0[0x13];
    unsigned char state;
    unsigned char b14;
    char pad15[7];
    char region1c[0xc];
    int word28;
};

struct Data021707d8_021583a8 {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
    unsigned char b4;
    char pad5[3];
    Blk_021583a8* ptr;
};
extern Data021707d8_021583a8 data_ov004_021707d8;
extern const char data_ov004_021703c0[];

TableA68* DispatchAdjustmentIfType4_02157018(void* obj, int key);
Messages_021583a8* GetGlobalField0x1c020421a0();
void ClearOrMaskNodesByState_0215781c(void* obj);
int GetSubWordField394_021b1ffc(Obj24_021b1ffc* obj);
unsigned char GetSubByteField398_021b2010(Obj24_021b2010* obj);
extern "C" void func_ov004_02157374(void* obj, int key, int cat, char* dst, unsigned char flag5);
extern "C" void func_0204500c(Messages_021583a8* msg, const char* text, int a, int b);
extern "C" void func_ov017_021b1e24(void* five, void* list);
int GetFieldromField_021b1fd4_021b1fd4(void* obj);
extern "C" void func_ov004_021571f4(void* obj, int val);
unsigned char GetSubByteField34_021b2024(Obj24_021b2024* obj);
unsigned char GetSubByteField35_021b2038(Obj24_021b2038* obj);
char* FindEntryByKey(TableA68* table, int key);
void CancelPendingAction020397cc(Obj020397cc* obj, int v);
void MaskNodesForRange_02157874(void* obj);
unsigned char GetSubByteField44_021b1fe8(Obj24_021b1fe8* obj);
void LoadBattleBlock020ac4c0(void* blk);
void AddClamped16BitLowAt0x20(S_a059c* blk, unsigned int v);
void CopyInBattleField0x7540(void* blk);
extern "C" void func_ov011_021848a0(void* obj, int val);
void SetXY1c0_02184be0(void* obj, short x, short y);

// USA: func_ov004_021583a8
extern "C" ARM int func_ov004_021583a8(void* a) {
    TableA68* table;
    Messages_021583a8* msg;
    int key;
    char* line;
    unsigned char kind;
    GameResources* res;
    void* list;
    unsigned char sub;
    int len;
    void* five;

    GameState::GetInstance();
    res = func_ov017_0218b5b0();
    list = res->unknown_ptr_array_36fc[0];
    five = res->unknown_ptr_array_3afc[19];
    table = DispatchAdjustmentIfType4_02157018(a, 4);
    msg = GetGlobalField0x1c020421a0();
    unsigned char state = data_ov004_021707d8.ptr->state;

    if (state == 2) {
        ClearOrMaskNodesByState_0215781c(a);
        key = GetSubWordField394_021b1ffc((Obj24_021b1ffc*)five);
        int cat = GetSubByteField398_021b2010((Obj24_021b2010*)five);
        if (key > 0 && data_ov004_021707d8.b4 == 0) {
            if (data_ov004_021707d8.b3 == 0) {
                line = msg->buf;
                memset(line, 0, 0x960);
                func_ov004_02157374(a, key, cat, line, 1);
                if ((signed char)line[0] != 0) {
                    if (line == strstr(line, data_ov004_021703c0)) {
                        line += 6;
                    }
                    func_0204500c(msg, line, 0, 0xe3);
                    msg->status = 1;
                    data_ov004_021707d8.b3 = 1;
                }
            } else {
                if (msg->mode != 0) return 1;
                data_ov004_021707d8.b4 = 1;
            }
        }
        func_ov017_021b1e24(five, list);
        int t = GetFieldromField_021b1fd4_021b1fd4(five) >> 12;
        if (data_ov004_021707d8.b4 == 0) {
            func_ov004_021571f4(a, t);
        }
        kind = GetSubByteField34_021b2024((Obj24_021b2024*)five);
        sub = GetSubByteField35_021b2038((Obj24_021b2038*)five);
        if (data_ov004_021707d8.b2 == 0 && kind == 4 && sub == 0) {
            func_0204500c(msg, FindEntryByKey(table, 0x2e), 0, 0xe3);
            msg->status = 1;
            data_ov004_021707d8.b2 = 1;
        }
        if (data_ov004_021707d8.b0 == 0) {
            if ((kind == 4 && sub == 5) || (data_ov004_021707d8.b4 != 0 && key > 0)) {
                if (data_ov004_021707d8.b4 == 0) {
                    func_0204500c(msg, FindEntryByKey(table, 0x25), 0, 0xe3);
                } else {
                    func_0204500c(msg, FindEntryByKey(table, 0x33), 0, 0xe3);
                }
                msg->status = 1;
                data_ov004_021707d8.b0 = 1;
            }
        }
        if (*(int*)(res->unknown_4490 + 4) == 0) {
            data_ov004_021707d8.ptr->state++;
            CancelPendingAction020397cc((Obj020397cc*)GameState::GetInstance()->GetUnknownGameObject(), 1);
        }
    } else if (state == 3) {
        if (msg->mode != 3) return 1;
        MaskNodesForRange_02157874(a);
        char* buf = msg->buf;
        memset(buf, 0, 0x960);
        char* name = 0;
        unsigned char flags = GetSubByteField44_021b1fe8((Obj24_021b1fe8*)five);
        if (flags & 1) {
            BattleBlock_021583a8 blk;
            LoadBattleBlock020ac4c0(&blk);
            AddClamped16BitLowAt0x20((S_a059c*)&blk, 1);
            CopyInBattleField0x7540(&blk);
            char* s = FindEntryByKey(table, 0x26);
            memcpy(buf, s, strlen(s));
            data_ov004_021707d8.ptr->b14 = 1;
            func_ov011_021848a0(a, 0x3af);
        } else {
            if (flags & 0x80) {
                name = FindEntryByKey(table, 0x35);
            } else if (data_ov004_021707d8.b2 != 0) {
                name = FindEntryByKey(table, 0x2f);
            }
            if (name != 0) {
                len = strlen(name);
                memcpy(buf, name, len);
                buf += len;
            }
            if (data_ov004_021707d8.b4 == 0) {
                int key = GetSubWordField394_021b1ffc((Obj24_021b1ffc*)five);
                int cat = GetSubByteField398_021b2010((Obj24_021b2010*)five);
                func_ov004_02157374(a, key, cat, buf, 0);
                buf += strlen(buf);
            }
            char* s = FindEntryByKey(table, 0x27);
            memcpy(buf, s, strlen(s));
            SetXY1c0_02184be0(a, 0x47, 0x46);
            data_ov004_021707d8.ptr->b14 = 0;
        }
        char* text = msg->buf;
        if (text == strstr(text, data_ov004_021703c0)) {
            text += 6;
        }
        func_0204500c(msg, text, 0, 0xe3);
        msg->status = 1;
        ((Five_021583a8*)five)->done = 1;
        data_ov004_021707d8.ptr->state = 0;
        Blk_021583a8* blk = data_ov004_021707d8.ptr;
        memset(blk->region1c, 0, 0xc);
        blk->word28 = 0;
        return 0;
    }
    return 1;
}
