#include <globaldefs.h>
#if defined(jpn)
enum {
    kRegion89c = 0x818
};
enum {
    kNodeOffset = 0x7f0
};
enum {
    kRegion101c = 0xf98
};
enum {
    kRegion1030 = 0xfac
};
enum {
    kRegion103a = 0xfb6
};
#else
enum {
    kRegion89c = 0x89c
};
enum {
    kNodeOffset = 0x874
};
enum {
    kRegion101c = 0x101c
};
enum {
    kRegion1030 = 0x1030
};
enum {
    kRegion103a = 0x103a
};
#endif

#include "GameState/GameState.h"

void *GetPtrField0x2a04(GameState *battleStruct);
struct Obj2081;
void ClearElementFlag0x20(struct Obj2081 *obj, int key);
void SetElementFlag0x20(struct Obj2081 *obj, int key);
void SetEntryLowNibbleAndElement02080c68(void *obj, int id, int value);
void SetSublistEntriesFlag1(void *obj, int id);
struct Container020dedd0;
struct Node021769ec;
struct Node021769ec *FindElementByKey020dedd0(struct Container020dedd0 *c, int key);
extern "C" int func_020dd4c4(int id, void *node);
struct Container02080f8c;
void SetEntryFirstField02080f8c(struct Container02080f8c *obj, int id, int value);
extern "C" int func_020dcc98(int combatantId);
struct Container02080cc0;
void SetEntryHighNibble0x13(struct Container02080cc0 *obj, int id, int value);
int SetEntryFlagById02080b40(void *obj, int id);
void SetEntryHalfword0xe(void *obj, int id, int value);
struct Field150Holder02052df8;
short GetHalfwordEntryFromField150(Field150Holder02052df8 *obj, int index);
extern "C" int func_ov003_021753a0(void *obj, int id, void *a, void *b);
struct Container02080fa8;
void SetEntryFirstField02080fa8(struct Container02080fa8 *obj, int id, int value);
struct Container02080fc4;
void SetEntryFirstField02080fc4(struct Container02080fc4 *obj, int id, int value);
extern "C" void func_020813ec(void *obj, int key);

extern int data_ov003_0217fb18[];

struct Node021769ec {
    char pad0[8];
    unsigned int kind : 4;
    unsigned int pad8 : 28;
    unsigned int lo : 7;
    unsigned int tier : 4;
    unsigned int hi : 21;
    char pad10[0x18 - 0x10];
    short count;
};

union FloatBits021769ec {
    float value;
    int bits;
};

struct Menu021769ec {
    char pad0[kNodeOffset];
    char nodes[kRegion89c - kNodeOffset];
    struct Obj2081 *elemObj;
    char pad8a0[kRegion101c - kRegion89c - 4];
    int ids[4];
    char pad102c[kRegion1030 - kRegion101c - 0x10];
    int memberCount;
    char pad1034[kRegion103a - kRegion1030 - 4];
    short key;
};

// USA: func_ov003_021769ec
extern "C" ARM void func_ov003_021769ec(Menu021769ec *self) {
    int listId;
    short idHp;
    short idCurA;
    short idMaxA;
    short idFlag;
    short idState;
    short idTier;
    short idCurB;
    short idMaxB;
    struct Obj2081 *elem;
    idHp    = 0x1c;
    idCurA  = 0x1d;
    listId  = 4;
    idFlag  = 0x1f;
    idState = 0x20;
    elem    = self->elemObj;
    idMaxA  = 0x1e;
    idTier  = 0x21;
    idCurB  = 0x145;
    idMaxB  = 0x146;
    GameState *battle    = GameState::GetInstance();
    GetPtrField0x2a04(battle);
    unsigned char count = self->memberCount;
    switch (count) {
        case 2:
            listId  = 5;
            idHp    = 0x22;
            idCurA  = 0x24;
            idMaxA  = 0x26;
            idFlag  = 0x28;
            idState = 0x2a;
            idTier  = 0x2c;
            idCurB  = 0x147;
            idMaxB  = 0x149;
            break;
        case 3:
            listId  = 6;
            idHp    = 0x2d;
            idCurA  = 0x30;
            idMaxA  = 0x33;
            idFlag  = 0x36;
            idState = 0x39;
            idTier  = 0x3c;
            idCurB  = 0x14b;
            idMaxB  = 0x14e;
            break;
        case 4:
            listId  = 7;
            idHp    = 0x3d;
            idCurA  = 0x41;
            idMaxA  = 0x45;
            idFlag  = 0x49;
            idState = 0x4d;
            idTier  = 0x51;
            idCurB  = 0x151;
            idMaxB  = 0x155;
            break;
    }

    ClearElementFlag0x20(elem, listId);
    SetEntryLowNibbleAndElement02080c68(elem, listId, 0);
    SetSublistEntriesFlag1(elem, listId);

    struct Node021769ec *node = FindElementByKey020dedd0((struct Container020dedd0 *) self->nodes, self->key);
    int show                  = 0;
    if (node != NULL) {
        int t = (node->kind <= 7) ? 1 : show;
        show  = (t != 0);
    }
    if (show != 0) {
        for (unsigned char i = 0; i < count; i++) {
            int ready;
            int cid;
            int hp;
            ready              = func_020dd4c4((signed char) self->ids[i], node);
            cid                = (signed char) self->ids[i];
            hp                 = 0;
            GameObject *member = battle->GetPartyMemberByIndex(cid);
            if (member != NULL) {
                hp = *(int *) ((char *) member + 0x134);
            }
            SetEntryFirstField02080f8c((struct Container02080f8c *) elem, idHp, hp);
            SetEntryHighNibble0x13((struct Container02080cc0 *) elem, idHp, func_020dcc98(cid));
            SetEntryFlagById02080b40(elem, idHp);
            if (ready != 0) {
                SetEntryHighNibble0x13((struct Container02080cc0 *) elem, idState, 3);
                SetEntryHalfword0xe(elem, idState, 0xb);
                SetEntryFlagById02080b40(elem, idState);
            } else {
                SetEntryHighNibble0x13((struct Container02080cc0 *) elem, idState, 0xf);
                int index = data_ov003_0217fb18[node->kind];
                if (index < 0) {
                    return;
                }
                short have = GetHalfwordEntryFromField150((Field150Holder02052df8 *) member, index);
                if (have == node->count) {
                    SetEntryHalfword0xe(elem, idState, 0x22);
                    SetEntryFlagById02080b40(elem, idState);
                } else {
                    unsigned short cur;
                    unsigned short max;
                    if (func_ov003_021753a0(self, cid, &cur, &max) != 0) {
                        int nibble = 0xf;
                        if (cur < max) {
                            nibble = 5;
                        }
                        if (cur > max) {
                            nibble = 9;
                        }
                        SetEntryHighNibble0x13((struct Container02080cc0 *) elem, idMaxA, nibble);
                        SetEntryFirstField02080fa8((struct Container02080fa8 *) elem, idCurA, cur);
                        SetEntryFirstField02080fa8((struct Container02080fa8 *) elem, idMaxA, max);
                        SetEntryFlagById02080b40(elem, idCurA);
                        SetEntryFlagById02080b40(elem, idMaxA);
                        SetEntryFlagById02080b40(elem, idFlag);
                    } else {
                        FloatBits021769ec fcur;
                        FloatBits021769ec fmax;
                        func_ov003_021753a0(self, cid, &fcur, &fmax);
                        int nibble = 0xf;
                        if (fcur.value < fmax.value) {
                            nibble = 5;
                        }
                        if (fcur.value > fmax.value) {
                            nibble = 9;
                        }
                        SetEntryHighNibble0x13((struct Container02080cc0 *) elem, idMaxB, nibble);
                        SetEntryFirstField02080fc4((struct Container02080fc4 *) elem, idCurB, fcur.bits);
                        SetEntryFirstField02080fc4((struct Container02080fc4 *) elem, idMaxB, fmax.bits);
                        SetEntryFlagById02080b40(elem, idCurB);
                        SetEntryFlagById02080b40(elem, idMaxB);
                        SetEntryFlagById02080b40(elem, idFlag);
                    }
                }
            }
            idHp++;
            idCurA++;
            idMaxA++;
            idFlag++;
            idState++;
            idCurB++;
            idMaxB++;
        }
        if (node->tier != 0) {
            SetEntryFlagById02080b40(elem, idTier);
            SetEntryHalfword0xe(elem, idTier, (short) (node->tier + 0x2f));
        }
    } else {
        SetElementFlag0x20(elem, listId);
    }
    func_020813ec(elem, listId);
}
