#include <globaldefs.h>
#include "GameState/GameState.h"

struct ShakeState0208ca00 {
    unsigned char pad0[0xd2];
    short field0xd2;
    unsigned char field0xd4;
    unsigned char pad1[0xd9 - 0xd5];
    signed char field0xd9;
    unsigned short flags0xda;
};

struct Pair0208bb78 {
    int a;
    int b;
};

struct Table0208bb78 {
    struct Pair0208bb78 entries[5];
};

int AnimateSubBG0Shake0208ca00(struct ShakeState0208ca00*, int);
extern "C" void func_0208bf78(void*);
void GetWord0x0(int*);
void ApplyValue0208bb78(int v);
extern int data_020e8ce4;
extern int data_020e6d5c;

// USA: func_0208bb78
ARM int UpdateScaledCombatantState0208bb78(struct ShakeState0208ca00* obj, int count) {
    if (obj->field0xd2 < 0) {
        return 1;
    }
    if (count == 0) {
        count = 1;
    }
    int ret = AnimateSubBG0Shake0208ca00(obj, (signed char)count);
    int d9 = obj->field0xd9;
    if (d9 != 0) {
        obj->field0xd9 = d9 - count;
        if (obj->field0xd9 < 0) {
            obj->field0xd9 = 0;
        }
    }
    if (obj->flags0xda & 0x20) {
        func_0208bf78(obj);
    } else {
        struct Table0208bb78 table = *(struct Table0208bb78*)&data_020e8ce4;
        table.entries[4].a = ((int*)&data_020e6d5c)[0];
        table.entries[4].b = ((int*)&data_020e6d5c)[1];
        int v = table.entries[obj->field0xd4].a;
        if (v == 0) {
            v = 1;
        }
        ApplyValue0208bb78(v);
    }
    GameState* battle = GameState::GetInstance();
    int word;
    GetWord0x0(&word);
    (void)battle;
    return ret;
}
