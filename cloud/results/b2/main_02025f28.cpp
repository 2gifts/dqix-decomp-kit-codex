#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct {
    int unk0;
    int unk4;
    struct CombatantStruct* combatantList[0xe9];
};
struct CombatantStruct {
    unsigned short flags;
    char unk[0x132];
    struct BaseCombatStats* baseStats;
    struct ModifiableCombatStats* currentStats;
};
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" struct CombatantStruct* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct* battleStruct);
extern "C" struct CombatantStruct* _ZN9GameState14GetProtagonistEv(struct BattleStruct* battleStruct);

extern "C" void* func_0205ec34(void);
void* GetData02105254(void);
extern "C" void* func_02012fe4(void);

struct Bcb8Params;
void ForwardParamsToB8bc(struct Bcb8Params* s, int arg1, int arg2);

struct Obj_0201bb78 { unsigned short field0; };
extern "C" int _Z31LookupBitFromValueRange0201bb78P12Obj_0201bb78j(struct Obj_0201bb78* obj, unsigned int val);

void _Z24SetElementFields0202756cPviiihhthii(void* obj, int p1, int p2, int idx,
        unsigned char p5, unsigned char p6, unsigned short p7, unsigned char p8, int p9, int p10);

int TestBitInByteArray(int unused, unsigned char* arr, int index);
extern "C" int _Z23HwDivideRounded020c2bf4jj(unsigned int numerHi, unsigned int denomLo);
extern "C" void _Z29ClampPositionToBounds0202630cPvPii(void* obj, int* pos, int margin);
extern "C" int _ZNK9GameState21GetEffectiveDeltaTimeEv(struct BattleStruct* battleStruct);

extern "C" int _u32_div_f(int a, int b);
extern "C" int _s32_div_f(int a, int b);
extern "C" int func_020017a4(int x);
void PackWordWithFlag0x40(unsigned int* out, unsigned char a, unsigned char b, int c, int d);

extern int data_020fdc4c;

struct Elem0xa9c {
    unsigned short h0;
    unsigned char b2;
    char pad3;
    int w4;
    int w8;
};

struct Vec3i { int x, y, z; };

struct G8Obj {
    char pad[0xc];
    unsigned char lowNibble : 4;
    unsigned char highNibble : 4;
};

struct GData {
    unsigned short id;
    char pad2[6];
    struct G8Obj* sub;
    char padC[0x38];
    int x44;
    int z48;
    char pad4c[0x2728];
    struct Vec3i pos;
};

struct BattleView {
    char pad0[0x20];
    char* link;
    char pad24[0x538];
    unsigned char resetPending;
    char pad55d[0x53c];
    unsigned char count;
    char pada9a[2];
    struct Elem0xa9c* elems;
};

// SCRATCH-USA: func_02025f28
extern "C" ARM void func_02025f28(void* receiver) {
    struct BattleView* obj = (struct BattleView*)receiver;
    struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();
    unsigned char* ctx = (unsigned char*)func_0205ec34();
    GetData02105254();
    struct GData* g = (struct GData*)func_02012fe4();
    unsigned short g0 = g->id;
    struct G8Obj* g8 = g->sub;
    struct CombatantStruct* c1 = _ZN9GameState20GetUnknownGameObjectEv(battle);
    _ZN9GameState14GetProtagonistEv(battle);
    struct Vec3i pos0 = *(struct Vec3i*)((char*)c1 + 0x44);

    if (obj->resetPending != 0) {
        obj->resetPending = 0;
        ForwardParamsToB8bc((struct Bcb8Params*)(obj->link + 8), 0, 0);
    }

    int flag2 = 1;
    for (int i = 0; i < obj->count; i++) {
        int result = _Z31LookupBitFromValueRange0201bb78P12Obj_0201bb78j((struct Obj_0201bb78*)g, (unsigned short)(obj->elems[i].h0 * 100));
        if (result == 0) continue;
        struct Elem0xa9c* e = obj->elems + i;
        int x = e->w4;
        int y = e->w8;
        if (obj->elems[i].b2 == 1) {
            _Z24SetElementFields0202756cPviiihhthii(obj, x + 0x7d000, y + 0x5d000, 0x18,
                (i + 0x55) & 0xff, 1, 0xff, 0xff, 0x1000, 0x1000);
        } else if (obj->elems[i].b2 == 2) {
            _Z24SetElementFields0202756cPviiihhthii(obj, x + 0x7d000, y + 0x5d000, 0x17,
                (i + 0x55) & 0xff, 0, 0xff, 0xff, 0x1000, 0x1000);
        }
    }

    int ok = 1;
    if (g0 == 10000 || TestBitInByteArray((int)ctx, ctx + 0x8c, 0x2b) == 0) flag2 = 0;
    int posVec[3];
    if (g0 == 10000 || g0 == 10100) {
        posVec[0] = pos0.x + 0x80000;
        posVec[2] = pos0.z + 0x60000;
    } else if (g8->lowNibble == 0) {
        posVec[0] = _Z23HwDivideRounded020c2bf4jj(pos0.x + g->x44, 0x6000) + 0x80000;
        posVec[2] = _Z23HwDivideRounded020c2bf4jj(pos0.z + g->z48, 0x6000) + 0x60000;
    } else {
        int g44 = g->x44;
        if (g44 == 0 || g->z48 == 0) {
            if (g44 == 0) ok = 0;
        } else {
            posVec[0] = g44 + 0x80000;
            posVec[2] = g->z48 + 0x60000;
        }
    }

    if (flag2) {
        struct Vec3i v = g->pos;
        v.x += 0x80000;
        v.z += 0x60000;
        _Z29ClampPositionToBounds0202630cPvPii(obj, (int*)&v, 0x7000);
        v.x -= 0x4000;
        v.z -= 0x4000;
        _Z24SetElementFields0202756cPviiihhthii(obj, v.x, v.z, 0x19, 0x54, 0, 0xff, 0xff, 0x1000, 0x1000);
    }

    int fieldVal = _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);
    unsigned int sum = *(unsigned int*)((char*)&data_020fdc4c + 0xc) + (unsigned int)fieldVal;
    unsigned int rem = sum % 0x560u;
    int q = ((int)rem - 0x2b0) / 0x2b;
    *(unsigned int*)((char*)&data_020fdc4c + 0xc) = rem;
    int absQ = func_020017a4(q);
    int q2 = (absQ * 0x4b) / 100;
    int c = q2 + 5;
    if (c > 0x10) c = 0x10;
    int d = 0x10 - c;
    PackWordWithFlag0x40((unsigned int*)0x04001050, 0, 3, c, d);

    _Z29ClampPositionToBounds0202630cPvPii(obj, posVec, 0x5000);
    posVec[0] -= 0x4000;
    posVec[2] -= 0x4000;

    if (ok) {
        short f4 = *(short*)((char*)c1 + 4);
        unsigned char p5 = (unsigned char)(f4 + 0x44);
        unsigned short p7 = (unsigned short)(f4 + 4);
        _Z24SetElementFields0202756cPviiihhthii(obj, posVec[0], posVec[2], 5, p5, 0, p7, 0xff, 0x1000, 0x1000);
    }
}
