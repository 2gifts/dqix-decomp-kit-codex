#include <globaldefs.h>
#include "GameState/GameState.h"
#include <std_library_functions.h>

struct ShakeState0208ca00;
typedef void (ShakeState0208ca00::*Handler0208bb78)();

struct ShakeState0208ca00 {
    unsigned char pad0[0xb4];
    unsigned char* particles;
    unsigned char pad1[0xcc - 0xb8];
    float factor;
    unsigned char pad2[0xd2 - 0xd0];
    short field0xd2;
    unsigned char field0xd4;
    unsigned char pad3[0xd8 - 0xd5];
    unsigned char field0xd8;
    signed char field0xd9;
    unsigned short flags0xda;
};

struct Values0208b788 {
    int x, y;
    float scale;
    unsigned short value;
    unsigned short pad;
};

struct SrcStruct0208baec {
    int x, y;
    float scale;
    unsigned short value;
    unsigned short pad;
};

struct DstStruct0208baec {
    unsigned char pad0[0x28];
};

struct HandlerTable0208bb78 {
    Handler0208bb78 e[5];
};

struct BattleData0208bb78 {
    unsigned char pad0[0x36fc];
    void* head;
};

struct HeadNode02046b24;
struct Struct0200fb08;
struct GetTableByteOrFFData;

int AnimateSubBG0Shake0208ca00(struct ShakeState0208ca00*, int);
extern "C" void func_0208bf78(void*);
extern "C" void func_0208b788(int index, float factor, Values0208b788* out, int adjust);
int SetupDstFromSrcAndScale0208baec(int a, DstStruct0208baec* dst, SrcStruct0208baec* src, int byteArg, float scaleArg,
                                    int pivot, unsigned char cArg);
BattleData0208bb78* GetWord0x0(int*);
int NormalizeField5_0200fb08(Struct0200fb08*);
int GetHeadNodeIdOrMinusOne(HeadNode02046b24**);
void* GetGlobal02109418(void);
unsigned char GetTableByteOrFF(GetTableByteOrFFData*, unsigned int);
int TestBitInByteArrayAt0x84(void*, int);
extern HandlerTable0208bb78 data_020e8ce4;
extern Handler0208bb78 data_020e6d5c;

// USA: func_0208bb78
ARM int UpdateScaledCombatantState0208bb78(struct ShakeState0208ca00* obj, int count) {
    if (obj->field0xd2 < 0) {
        return 1;
    }
    if (count == 0) {
        count = 1;
    }
    int ret = AnimateSubBG0Shake0208ca00(obj, (signed char)count);
    if (obj->field0xd9 != 0) {
        obj->field0xd9 = obj->field0xd9 - count;
        if (obj->field0xd9 < 0) {
            obj->field0xd9 = 0;
        }
    }
    if (obj->flags0xda & 0x20) {
        func_0208bf78(obj);
    } else {
        HandlerTable0208bb78 table = data_020e8ce4;
        table.e[4] = data_020e6d5c;
        if (table.e[obj->field0xd4] == 0) {
            return 1;
        }
        (obj->*table.e[obj->field0xd4])();
    }
    GameState* gs = GameState::GetInstance();
    BattleData0208bb78* battle = GetWord0x0((int*)gs);
    float scale;
    HeadNode02046b24** head = (HeadNode02046b24**)battle->head;
    if (obj->flags0xda & 8) {
        int f = (obj->flags0xda & 0x100) ? 1 : 0;
        SrcStruct0208baec a;
        func_0208b788(obj->field0xd8, obj->factor, (Values0208b788*)&a, f);
        scale = a.scale;
        if (obj->field0xd8 == 0) {
            scale = scale * scale;
        }
        unsigned char idx = 0;
        if (f) {
            idx = 1;
        }
        if (obj->flags0xda & 0x100) {
            a.y -= 0x10;
            SetupDstFromSrcAndScale0208baec((int)obj + 0x60, (DstStruct0208baec*)(obj->particles + idx * 0x28), &a, 0x20,
                                            scale, ret, 1);
            SrcStruct0208baec b;
            memcpy(&b, &a, sizeof(b));
            b.y += 0x38;
            if (NormalizeField5_0200fb08((Struct0200fb08*)gs) == 5) {
                b.y += 0x10;
            }
            SetupDstFromSrcAndScale0208baec((int)obj + 0x60,
                                            (DstStruct0208baec*)(obj->particles + (unsigned char)(idx + 1) * 0x28), &b,
                                            0x30, scale, ret, 1);
        } else {
            int ok = 1;
            if (GetHeadNodeIdOrMinusOne(head) == 0x2d) {
                idx = 1;
            } else if (obj->field0xd8 == 1) {
                ok = 0;
            }
            if (ok) {
                SetupDstFromSrcAndScale0208baec((int)obj + 0x60, (DstStruct0208baec*)(obj->particles + idx * 0x28), &a,
                                                0x40, scale, ret, 1);
            }
        }
    }
    if (obj->field0xd2 != 9999 && obj->field0xd2 >= 0 && obj->field0xd4 != 0 && obj->field0xd4 != 3 &&
        GetHeadNodeIdOrMinusOne(head) != 0x2d) {
        void* g = GetGlobal02109418();
        unsigned char tb = GetTableByteOrFF((GetTableByteOrFFData*)g, (unsigned char)obj->field0xd2);
        if (TestBitInByteArrayAt0x84(g, tb)) {
            SrcStruct0208baec c;
            func_0208b788(1, 1.0f, (Values0208b788*)&c, 0);
            SetupDstFromSrcAndScale0208baec((int)obj + 0x60, (DstStruct0208baec*)(obj->particles + 0x28), &c, 0x20,
                                            c.scale, ret, 2);
        }
    }
    return 0;
}
