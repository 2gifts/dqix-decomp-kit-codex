#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Combat/Main/BattleList.h"

unsigned char CopyOutRegion0x571d(char* obj, void* dst);
int CheckField0x56bLowNibble(struct Obj02061bd8* obj);
unsigned short GetFlagBitFromField150_0215eb20(void* obj, int bit);
struct Obj0205eaa0;
extern struct Obj0205eaa0 data_02108760;
void DispatchWithShortB4_0205eaa0(struct Obj0205eaa0* obj, int a, int b);
void* CheckNodeState6_0215e4c0(void* a, int key);
struct Obj021f809c;
extern "C" void func_ov023_021f809c(struct Obj021f809c*, void*);
extern "C" void func_ov023_021fc71c(void*, int, int);
extern "C" void func_020dc7e8(int, int);
extern "C" void func_ov004_0215ee90(void*);
extern "C" void func_ov004_0215f0c0(void*);
extern "C" void func_ov004_0215e5fc(void*, int);
extern "C" void func_ov004_0215eb40(void*);
extern "C" void func_ov004_0215ef14();
extern "C" void func_ov004_0216033c(void*);
extern "C" void func_ov004_02161514(void*);
extern "C" void func_ov004_02161c18(void*);
extern "C" void func_ov004_02162108(void*);
extern "C" void func_ov004_0216276c(void*);
void CallVTableFnAt240_021f66bc(void* a, int b, void* c);

struct Sub950 {
    char pad[0x950];
    int field950;
};

struct FlagBitHolder {
    char pad[0x150];
    Sub950* sub;
};

class Node0215e47c {
public:
    char pad4[0xc];
    unsigned char flags;
};

struct NodeShorts {
    char pad[0x104];
    short s104;
    short s106;
};

extern "C" Node0215e47c* func_ov004_0215e47c(void* a, int key);

struct CursorState {
    unsigned char busy;
    unsigned char f1;
    unsigned char f2;
    unsigned char f3;
    unsigned char f4;
    signed char f5;
    signed char f6;
    unsigned char type;
    unsigned char entry;
    signed char f9;
    unsigned char index;
    signed char fb;
    char pad[0x10];
    int combatant;
    char pad2[0x4c];
    int f6c;
    int f70;
};

extern CursorState data_ov004_021707e8;
extern int data_ov004_0216fe5c[];
extern int data_ov004_02170854[];
extern unsigned char data_ov004_02170838[];
extern unsigned char data_ov004_02170845[];
extern unsigned char data_ov004_02170b90[];

// USA: func_ov004_0215fdfc
extern "C" ARM int func_ov004_0215fdfc(void* a) {
    if (data_ov004_021707e8.busy == 1) return 0;
    data_ov004_021707e8.busy = 1;
    GameState* gs;
    GameObject* c;
    unsigned char saved;
    int none;
    int target;
    int n;
    unsigned char order[4];
    gs = GameState::GetInstance();
    n = CopyOutRegion0x571d((char*)gs, order);
    target = data_ov004_021707e8.combatant;
    none = 0;
    saved = data_ov004_021707e8.index;
    if (GetCombatantWithFlag0x100(gs, target) == NULL) none = 1;
    if (!none) {
        data_ov004_021707e8.index = data_ov004_021707e8.index + 1;
        data_ov004_021707e8.index = (unsigned)data_ov004_021707e8.index % n;
    }
    data_ov004_021707e8.combatant = order[data_ov004_021707e8.index];
    c = GetCombatantWithFlag0x100(gs, data_ov004_021707e8.combatant);
    if (data_ov004_021707e8.combatant == target && !none) {
        c = NULL;
        while (c == NULL) {
            data_ov004_021707e8.index = data_ov004_021707e8.index + 1;
            data_ov004_021707e8.index = (unsigned)data_ov004_021707e8.index % n;
            data_ov004_021707e8.combatant = order[data_ov004_021707e8.index];
            if (data_ov004_021707e8.combatant == target) {
                data_ov004_021707e8.index = saved;
                data_ov004_021707e8.combatant = target;
                data_ov004_021707e8.busy = 0;
                Node0215e47c* nd = func_ov004_0215e47c(a, 0x85);
                if (nd) nd->flags |= 0x78;
                return 0;
            }
            c = GetCombatantWithFlag0x100(gs, data_ov004_021707e8.combatant);
            if (c != NULL) break;
        }
    }
    if (c == NULL) {
        while (c == NULL) {
            data_ov004_021707e8.index = data_ov004_021707e8.index + 1;
            data_ov004_021707e8.index = (unsigned)data_ov004_021707e8.index % n;
            data_ov004_021707e8.combatant = order[data_ov004_021707e8.index];
            if (data_ov004_021707e8.combatant == target) {
                data_ov004_021707e8.index = saved;
                data_ov004_021707e8.combatant = target;
                data_ov004_021707e8.busy = 0;
                Node0215e47c* nd = func_ov004_0215e47c(a, 0x85);
                if (nd) nd->flags |= 0x78;
                return 0;
            }
            c = GetCombatantWithFlag0x100(gs, data_ov004_021707e8.combatant);
            if (c != NULL) break;
        }
    }
    if (CheckField0x56bLowNibble((struct Obj02061bd8*)c)) {
        int ok = 0;
        while (!ok) {
            data_ov004_021707e8.index = data_ov004_021707e8.index + 1;
            data_ov004_021707e8.index = (unsigned)data_ov004_021707e8.index % n;
            data_ov004_021707e8.combatant = order[data_ov004_021707e8.index];
            if (data_ov004_021707e8.combatant == target) {
                data_ov004_021707e8.index = saved;
                data_ov004_021707e8.combatant = target;
                data_ov004_021707e8.busy = 0;
                return 0;
            }
            c = GetCombatantWithFlag0x100(gs, data_ov004_021707e8.combatant);
            if (c == NULL) {
                data_ov004_021707e8.index = saved;
                data_ov004_021707e8.combatant = target;
                break;
            }
            if (CheckField0x56bLowNibble((struct Obj02061bd8*)c) == 0) ok = 1;
        }
    }
    int v = ((FlagBitHolder*)c)->sub->field950;
    data_ov004_021707e8.f2 = v;
    data_ov004_021707e8.entry = 0;
    data_ov004_021707e8.f6c = 0;
    int key = v & 0xff;
    data_ov004_021707e8.f70 = key;
    unsigned char i = 1;
    unsigned char j = 2;
    while (j < 0xd) {
        int t = data_ov004_0216fe5c[i];
        data_ov004_02170854[j] = t;
        if (key != t) {
            j++;
            i++;
        } else {
            i++;
        }
    }
    int e = data_ov004_021707e8.entry;
    while (key != data_ov004_02170854[(unsigned char)e]) {
        data_ov004_021707e8.entry = (unsigned char)e + 1;
        e = data_ov004_021707e8.entry % 13;
        data_ov004_021707e8.entry = e;
        if (key == data_ov004_02170854[data_ov004_021707e8.entry]) break;
    }
    for (int k = 0; k < 13; k++) {
        if (GetFlagBitFromField150_0215eb20(c, data_ov004_02170854[k]) != 0) data_ov004_02170838[k] = 1;
        else data_ov004_02170838[k] = 0;
        if (GetFlagBitFromField150_0215eb20(c, data_ov004_0216fe5c[k]) != 0) data_ov004_02170845[k] = 1;
        else data_ov004_02170845[k] = 0;
    }
    data_ov004_021707e8.f3 = data_ov004_021707e8.entry;
    func_ov004_0215ee90(data_ov004_02170b90);
    data_ov004_021707e8.f9 = 0;
    data_ov004_021707e8.f5 = 0;
    func_ov004_0215f0c0(a);
    func_ov004_0215e5fc(a, data_ov004_021707e8.combatant);
    func_ov004_0215eb40(a);
    func_ov004_0215ef14();
    data_ov004_021707e8.f1 = 1;
    DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
    Node0215e47c* nd = func_ov004_0215e47c(a, 0x82);
    func_ov023_021fc71c(nd, data_ov004_021707e8.combatant, 1);
    data_ov004_021707e8.fb = 0;
    data_ov004_021707e8.f6 = 0;
    switch (data_ov004_021707e8.type - 0x64) {
    case 0:
        data_ov004_021707e8.f6 = data_ov004_021707e8.f9;
        func_ov004_0216033c(a);
        break;
    case 1:
        data_ov004_021707e8.f6 = data_ov004_021707e8.f5;
        func_ov004_02161514(a);
        break;
    case 2:
        data_ov004_021707e8.f6 = data_ov004_02170b90[0x23f];
        func_ov004_02161c18(a);
        break;
    case 3:
        data_ov004_021707e8.f6 = data_ov004_02170b90[0x47f];
        func_ov004_02162108(a);
        break;
    case 4:
        data_ov004_021707e8.f6 = 1;
        func_ov004_0216276c(a);
        break;
    }
    func_020dc7e8(2, (signed char)data_ov004_021707e8.combatant);
    func_020dc7e8(5, (signed char)data_ov004_021707e8.combatant);
    NodeShorts* node = (NodeShorts*)CheckNodeState6_0215e4c0(a, 3);
    node->s104 = data_ov004_021707e8.fb;
    node->s106 = data_ov004_021707e8.f6;
    func_ov023_021f809c((struct Obj021f809c*)node, a);
    data_ov004_021707e8.busy = 0;
    CallVTableFnAt240_021f66bc(a, 0x85, (void*)1);
    return 0;
}
