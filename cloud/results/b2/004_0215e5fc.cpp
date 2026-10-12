#include <globaldefs.h>
#include "GameState/GameState.h"

struct NodePos { int x, y, z; };

class ListNode {
public:
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06();
    virtual void SetPosition(NodePos* pos);
    virtual NodePos GetPosition();
    virtual void v09(); virtual void v10(); virtual void v11(); virtual void v12();
    virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16();
    virtual void v17(); virtual void v18(); virtual void v19(); virtual void v20();
    virtual void v21(); virtual void v22(); virtual void v23(); virtual void v24();
    virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28();
    virtual void v29(); virtual void v30(); virtual void v31(); virtual void v32();
    virtual void v33(); virtual void v34(); virtual void v35(); virtual void v36();
    virtual void v37(); virtual void v38(); virtual void v39(); virtual void v40();
    virtual void v41(); virtual void v42(); virtual void v43(); virtual void v44();
    virtual void v45(); virtual void v46(); virtual void v47(); virtual void v48();
    virtual void v49(); virtual void v50(); virtual void v51(); virtual void v52();
    virtual void v53();
    virtual void SetPalette(int palette);
    virtual void v55(); virtual void v56(); virtual void v57();
    virtual struct Container020dedd0* GetContainer();

    unsigned char pad4[8];
    unsigned char flags;
    unsigned char padd[0x13];
    char* text;
    unsigned char pad24[0x21];
    unsigned char flags45;
};

struct ElementFlags { unsigned int pad : 7; unsigned int style : 4; };

struct Element020dedd0 {
    struct ElementFlags* info;
    void* resource;
    unsigned char pad8[0x10];
    short id;
};

struct BattleMenuState {
    unsigned char pad0[0x18];
    char* buffers;
};

struct Field150 {
    unsigned char pad0[0x488];
    short ids[1];
};

unsigned char* GetFieldAt0x150(unsigned char* obj);
struct Container020dedd0;
struct Element020dedd0* FindElementByKey020dedd0(struct Container020dedd0* c, int key);
int GetGlobalField0x1c020421a0(void);
int FindSubstring(char* hay, char* needle);
void* CheckNodeState6_0215e4c0(void* a, int key);
extern "C" void _Z27SetOffset100Fields_021f8960Pcss(void* node, unsigned short a, unsigned short b);

extern "C" void* func_ov011_021849c8(void* a);
extern "C" ListNode* func_ov004_0215e47c(void* a, int key);
extern "C" ListNode* func_ov023_021f6880(void* list, int key);
extern "C" int func_ov023_021f6f10(ListNode* node);
extern "C" void func_02083738(void* obj, int a);
extern "C" void func_0208358c(void* obj, struct Element020dedd0* e, int a);
extern "C" void func_02083e28(void* obj, int a);
extern "C" void func_02046608(int a, int b, void* fmt, void* dst, int p4, int p5, int p6);
extern "C" void func_ov023_021f809c(void* obj, void* param2);

extern int data_ov004_0216fe3c[];
extern short data_ov004_02170888[][4];
extern unsigned char data_ov004_0216fdfc[];
extern short data_ov004_02170818[];
extern unsigned char data_ov004_02170814[];
extern struct BattleMenuState data_ov004_021707e8;
extern char* data_ov004_021704a4[][4];
extern char data_ov004_02170524[];
extern char data_ov004_02170525[];

// USA: func_ov004_0215e5fc
extern "C" ARM void func_ov004_0215e5fc(void* self, int slot) {
    void* list = func_ov011_021849c8(self);
    short* ids;
    GameState* gs = GameState::GetInstance();
    int found = 0;
    void* combatant = GetCombatantWithFlag0x100(gs, slot);
    if (combatant) {
        Field150* field = (Field150*)GetFieldAt0x150((unsigned char*)combatant);
        ListNode* root = func_ov004_0215e47c(self, 0x81);
        if (root) {
            Container020dedd0* cont = root->GetContainer();
            if (cont) {
                ids = field->ids;
                        for (int i = 0; i < 8; i++) {
                    short cur = ids[data_ov004_0216fe3c[i]];
                    if (cur != data_ov004_02170888[i][slot] || data_ov004_0216fdfc[i] == 1) {
                        found = 1;
                        Element020dedd0* elem = FindElementByKey020dedd0(cont, cur);
                        if (!elem) {
                            int kind = data_ov004_0216fdfc[i];
                            func_02083738(field, kind);
                            data_ov004_02170888[i][slot] = -1;
                            if (cur < 0) {
                                if (i == 0) {
                                    data_ov004_02170818[slot] = 0xe;
                                }
                            }
                            int key = -1;
                            int arg = -1;
                            if (kind == 2) {
                                key = 1000;
                                arg = 0;
                            }
                            Element020dedd0* other = FindElementByKey020dedd0(cont, key);
                            if (other) {
                                func_0208358c(field, other, arg);
                            }
                            data_ov004_02170814[slot] = 0;
                            continue;
                        }
                        char* buf = data_ov004_021707e8.buffers + ((i + slot * 8) << 7);
                        memset(buf, 0, 0x80);
                        func_02046608(GetGlobalField0x1c020421a0(), 0, elem->resource, buf, 0x60, 0, 0);
                        func_0208358c(field, elem, -1);
                        data_ov004_02170888[i][slot] = elem->id;
                        data_ov004_021704a4[i][slot] = buf;
                        if (i == 0) {
                            int style = elem->info->style;
                            if (style == 0) {
                                style = 0xe;
                            }
                            data_ov004_02170818[slot] = style;
                        }
                        if (i == 1) {
                            data_ov004_02170814[slot] = 0;
                            if (elem->id != -1) {
                                data_ov004_02170814[slot] = 1;
                            }
                        }
                    }
                }
                if (found) {
                    func_02083e28(field, 0);
                }
            }
        }
    }

    for (int n = 0; n < 8; n++) {
        ListNode* node = func_ov023_021f6880(list, n + 0xfa);
        if (node && func_ov023_021f6f10(node) == 8) {
            int v = data_ov004_02170888[n][slot];
            if (v < 10000 || (v >= 14000 && v < 15000)) {
                node->text = data_ov004_02170524;
                ListNode* label = func_ov023_021f6880(list, n + 0x141);
                if (label) {
                    label->flags &= ~8;
                    label->SetPalette(3);
                }
            } else {
                char* str = data_ov004_021704a4[n][slot];
                NodePos pos = node->GetPosition();
                int y = (n * 20) << 12;
                if (FindSubstring(str, data_ov004_02170525)) {
                    y -= 0x5000;
                }
                pos.y = y + 0x15000;
                node->SetPosition(&pos);
                node->text = str;
                node->flags45 |= 0x80;
                node->SetPalette(0xf);
                ListNode* label = func_ov023_021f6880(list, n + 0x141);
                if (label) {
                    label->flags |= 8;
                }
            }
        }
    }

    void* tail = CheckNodeState6_0215e4c0(self, 4);
    _Z27SetOffset100Fields_021f8960Pcss(tail, 0, 0xffff);
    func_ov023_021f809c(tail, self);
}
