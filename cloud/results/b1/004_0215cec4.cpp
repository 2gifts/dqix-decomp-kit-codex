#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Combat/Main/MessageSlotTable.h"

struct Battle021707d8_0215cec4 { char pad[0x32]; short members[4]; short memberCount; };
struct Data021707d8_0215cec4 { void* field0; void* field4; Battle021707d8_0215cec4* battle; };
extern Data021707d8_0215cec4 data_ov004_021707d8[];

struct MemberIds_0215cec4 { int ids[4]; };
extern MemberIds_0215cec4 data_ov004_0216fb70;

extern "C" int _Z28DispatchNodeIfType7_02156e2cPvi(void* a, int key);
struct Entry_0215cec4 { short key; };
extern "C" Entry_0215cec4* _Z20GetEntryFor_021570a4Pvi(void* obj, int index);
struct Container020dedd0;
struct Element020de650 { char pad[8]; unsigned int kind : 4; };
extern "C" Container020dedd0* func_ov004_02156fd4(void* obj, int key);
Element020de650* FindElementByKey020dedd0(Container020dedd0* c, int key);
extern "C" void func_020dd7ac(void* buf);
extern "C" void* func_ov004_02156f04(void* a, int key);
extern "C" int func_020dcc98(signed char combatantId);
int IsValueInCombatantField150List(int combatantId, int value);
extern "C" void func_ov023_021f65d4(void* obj, int id, int mask);
void ClearNodeMaskById_021f6600(void* obj, int id, int mask);
extern "C" int func_020dd4c4(int id, void* node);
void Setup020ddaf4(int a, int b, int c, int d, int e, int f);
extern "C" void* func_ov004_02156f38(void* obj, int key);
extern "C" void* func_ov011_021849c8(void* a);
extern "C" void* func_ov023_021f6880(void* obj, int key);
extern "C" void* func_ov004_02156f6c(void* obj, int key);
extern "C" void func_ov023_021f809c(void* obj, void* param2);

typedef void (*FuncPtrE0_0215cec4)(void*, int);
struct HandlerE0_0215cec4 { char pad[0xe0]; FuncPtrE0_0215cec4 func; };
struct NodeE0_0215cec4 { HandlerE0_0215cec4* handler; };

typedef void (*FuncPtrD8_0215cec4)(void*, int);
struct HandlerD8_0215cec4 { char pad[0xd8]; FuncPtrD8_0215cec4 func; };
struct NodeD8_0215cec4 { HandlerD8_0215cec4* handler; };

class MemberPanel_0215cec4 {
public:
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39();
    virtual void v40(); virtual void v41(); virtual void v42(); virtual void v43();
    virtual void v44(); virtual void v45(); virtual void v46(); virtual void v47();
    virtual void v48(); virtual void v49(); virtual void v50(); virtual void v51();
    virtual void v52(); virtual void v53();
    virtual void SetIcon(int icon);
    char pad[0x1c];
    BaseCombatStats* stats;
};
struct TitlePanel_0215cec4 { char pad[0x38]; short style; };

// USA: func_ov004_0215cec4
extern "C" ARM int func_ov004_0215cec4(void* a) {
    GameState* bs = GameState::GetInstance();
    int key = _Z28DispatchNodeIfType7_02156e2cPvi(a, 0x5b);
    if (key < 0) return 0;
    Entry_0215cec4* entry = _Z20GetEntryFor_021570a4Pvi(a, key & 0xff);
    if (!entry) return 0;
    Container020dedd0* node = func_ov004_02156fd4(a, 5);
    if (!node) return 0;
    Element020de650* elem = FindElementByKey020dedd0(node, entry->key);
    if (!elem) return 0;

    MessageSlotTable_020dd7ac buf;
    func_020dd7ac(&buf);
    int typeCode = 1;
    if (elem->kind != 0) typeCode = 3;

    TitlePanel_0215cec4* title = (TitlePanel_0215cec4*)func_ov004_02156f04(a, 0x1d6);
    if (title) title->style = typeCode + 0xb;

    MemberIds_0215cec4 members = data_ov004_0216fb70;
    Data021707d8_0215cec4* data = data_ov004_021707d8;
    int count;
    int i;
    Battle021707d8_0215cec4* battle = data->battle;
    count = battle->memberCount;
    for (i = 0; i < count; i++) {
        members.ids[i] = battle->members[i];
    }

    for (unsigned char j = 0; j < 4; j++) {
        int id = members.ids[j];
        GameObject* member = bs->GetPartyMemberByIndex(id);
        if (member) {
            MemberPanel_0215cec4* stat = (MemberPanel_0215cec4*)func_ov004_02156f04(a, j + 0x1c2);
            if (!stat) return 0;
            stat->stats = member->baseStats_;
            stat->SetIcon(func_020dcc98(members.ids[j]));
            if (IsValueInCombatantField150List((signed char)members.ids[j], entry->key)) {
                func_ov023_021f65d4(a, j + 0x1c6, 8);
                func_ov023_021f65d4(a, j + 0x1ce, 8);
                func_ov023_021f65d4(a, j + 0x1ca, 8);
                func_ov023_021f65d4(a, j + 0x1d2, 8);
                ClearNodeMaskById_021f6600(a, j + 0x1d7, 8);
            } else if (func_020dd4c4((signed char)id, elem)) {
                func_ov023_021f65d4(a, j + 0x1c6, 8);
                func_ov023_021f65d4(a, j + 0x1ce, 8);
                func_ov023_021f65d4(a, j + 0x1ca, 8);
                ClearNodeMaskById_021f6600(a, j + 0x1d2, 8);
                func_ov023_021f65d4(a, j + 0x1d7, 8);
            } else {
                ClearNodeMaskById_021f6600(a, j + 0x1c6, 8);
                ClearNodeMaskById_021f6600(a, j + 0x1ce, 8);
                ClearNodeMaskById_021f6600(a, j + 0x1ca, 8);
                func_ov023_021f65d4(a, j + 0x1d2, 8);
                func_ov023_021f65d4(a, j + 0x1d7, 8);

                func_020dd7ac(&buf);
                unsigned short valA, valB;
                if (elem->kind == 0) {
                    Setup020ddaf4((int)&buf, id, (int)elem, 0, (int)&valA, (int)&valB);
                } else {
                    Setup020ddaf4((int)&buf, id, (int)elem, 1, (int)&valA, (int)&valB);
                }

                NodeE0_0215cec4* hA = (NodeE0_0215cec4*)func_ov004_02156f38(a, j + 0x1c6);
                if (hA) hA->handler->func(hA, valA);
                NodeE0_0215cec4* hB = (NodeE0_0215cec4*)func_ov004_02156f38(a, j + 0x1ca);
                if (hB) hB->handler->func(hB, valB);

                NodeD8_0215cec4* dA = (NodeD8_0215cec4*)func_ov023_021f6880(func_ov011_021849c8(a), j + 0x1c6);
                if (dA) dA->handler->func(dA, 0xf);

                if (valA == valB) {
                    NodeD8_0215cec4* dB = (NodeD8_0215cec4*)func_ov023_021f6880(func_ov011_021849c8(a), j + 0x1ca);
                    if (dB) dB->handler->func(dB, 0xf);
                } else if (valA < valB) {
                    NodeD8_0215cec4* dB = (NodeD8_0215cec4*)func_ov023_021f6880(func_ov011_021849c8(a), j + 0x1ca);
                    if (dB) dB->handler->func(dB, 5);
                } else {
                    NodeD8_0215cec4* dB = (NodeD8_0215cec4*)func_ov023_021f6880(func_ov011_021849c8(a), j + 0x1ca);
                    if (dB) dB->handler->func(dB, 9);
                }
            }
        } else {
            func_ov023_021f65d4(a, j + 0x1c2, 8);
            func_ov023_021f65d4(a, j + 0x1c6, 8);
            func_ov023_021f65d4(a, j + 0x1ce, 8);
            func_ov023_021f65d4(a, j + 0x1ca, 8);
            func_ov023_021f65d4(a, j + 0x1d2, 8);
            func_ov023_021f65d4(a, j + 0x1d7, 8);
        }
    }

    void* n17 = func_ov004_02156f6c(a, 0x17);
    if (n17) func_ov023_021f809c(n17, a);
    return 0;
}
