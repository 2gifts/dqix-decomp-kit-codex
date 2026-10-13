#include <globaldefs.h>

struct GameState;

struct ItemDef0215c530
{
    unsigned int a : 7;
    unsigned int b : 4;
    unsigned int pad0 : 18;
    unsigned int d : 1;
    unsigned int e : 2;
    unsigned int c : 12;
    unsigned int pad1 : 15;
    unsigned int f : 1;
    unsigned int g : 1;
    unsigned int h : 1;
    unsigned int pad2 : 2;
    char pad3[0x20 - 8];
};

struct Item0215c530
{
    ItemDef0215c530* def;
    int f4;
    unsigned int equipType : 4;
    unsigned int pad0 : 28;
    char pad1[0x18 - 0xc];
    short id;
    char pad2[0x20 - 0x1a];
};

struct Entry0215c530
{
    short id;
    unsigned short equipType : 4;
    unsigned short pad0 : 12;
    unsigned int a : 7;
    unsigned int b : 4;
    unsigned int c : 12;
    unsigned int d : 1;
    unsigned int e : 2;
    unsigned int f : 1;
    unsigned int g : 1;
    unsigned int h : 1;
    unsigned int pad1 : 3;
};

struct ListNode0215c530
{
    ListNode0215c530* next;
    char pad[4];
    short id;
};

struct Slot0215c530
{
    short item;
    char pad[0x1c - 2];
};

struct Container0215c530
{
    int unk[2];
};

struct Sub0215c530
{
    char pad0[0x1000];
    Container0215c530 container;
    char pad1[0x179c - 0x1000 - sizeof(Container0215c530)];
};

struct Menu0215c530
{
    char pad0[0x244];
    Sub0215c530 sub;
    ListNode0215c530* head;
    char pad1[0x2d90 - 0x19e4];
    Slot0215c530 slots[24];
    char pad2[0x3d98 - 0x3030];
    short counts[8];
    char pad3[0x3db9 - 0x3da8];
    unsigned char f3db9;
    char pad4[0x3dbb - 0x3dba];
    signed char selected;
    unsigned char tab;
    signed char page;
    char pad5;
    signed char prevPage;
    char pad6[0x3dcc - 0x3dc0];
    unsigned int dirty;
    char pad7[0x3dd4 - 0x3dd0];
    unsigned char modes[8];
};

struct GameResources0215c530
{
    char pad[0x3708];
    int* ptr;
};

struct Member0215c530
{
    char pad[0x4fc];
    int member;
};

struct StructDE1D4;
struct Obj0205eaa0;

extern "C" GameState* _ZN9GameState11GetInstanceEv();
extern "C" unsigned char* _Z17GetPtrField0x2a04P9GameState(GameState*);
extern "C" short* _Z24GetPointerFromArray0xbd0Phj(unsigned char*, unsigned int);
extern "C" signed char* _Z17GetPointerAt0xbf0Pvj(void*, unsigned int);
extern "C" void _Z15Compact0207c51cPhi(unsigned char*, int);
extern "C" void _Z22ClearElements_021e1518Pv(void*);
extern "C" void _Z19BuildChain_021e1b5cPviii(void*, int, int, int);
extern "C" void _Z19BuildChain_021e1b0cPviii(void*, int, int, int);
extern "C" GameResources0215c530* func_ov017_0218b5b0();
extern "C" Member0215c530* _Z19GetField1c_021a193cPi(int*);
extern "C" void _Z18InitStruct020de1d4P11StructDE1D4(StructDE1D4*);
extern "C" void* memset(void*, int, unsigned int);
extern "C" void* memcpy(void*, const void*, unsigned int);
extern "C" Entry0215c530* _Z22FindEntryById_021db08cPvi(void*, int);
extern "C" int func_020dd4c4(signed char, Item0215c530*);
extern "C" void func_ov005_021579ec(Menu0215c530*, int, int);
extern "C" void func_0205bb04(void*, int);
extern "C" void _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(void*, int, int);
extern "C" void func_ov005_021555c0(Menu0215c530*);
extern "C" void func_ov023_021dcae0(Container0215c530*, short);
extern "C" void func_ov005_0215792c(Menu0215c530*, int);

extern unsigned char data_ov005_0215cd08[];
extern unsigned char data_ov005_0215cd00[];
extern char data_02108760[];

// USA: func_ov005_0215c530
extern "C" ARM void func_ov005_0215c530(Menu0215c530* self)
{
    unsigned char* bag = _Z17GetPtrField0x2a04P9GameState(_ZN9GameState11GetInstanceEv());
    int count = self->counts[self->tab];
    short* ids = _Z24GetPointerFromArray0xbd0Phj(bag + 0x1d4, data_ov005_0215cd08[self->tab]);
    signed char* amounts = _Z17GetPointerAt0xbf0Pvj(bag + 0x1d4, data_ov005_0215cd08[self->tab]);
    _Z15Compact0207c51cPhi(bag + 0x1d4, data_ov005_0215cd00[self->tab]);

    int rebuild = 0;
    int reorder = 0;
    if (self->modes[self->tab] == 0 || self->modes[self->tab] == 1)
    {
        _Z22ClearElements_021e1518Pv(&self->head);
        _Z19BuildChain_021e1b5cPviii(&self->head, -1, data_ov005_0215cd00[self->tab], -1);
        rebuild = 1;
    }
    else if (self->modes[self->tab] == 3)
    {
        _Z22ClearElements_021e1518Pv(&self->head);
        _Z19BuildChain_021e1b0cPviii(&self->head, -1, data_ov005_0215cd00[self->tab], -1);
        rebuild = 1;
    }
    else if (self->modes[self->tab] == 2)
    {
        reorder = 1;
        rebuild = 1;
    }

    if (rebuild)
    {
        int target = self->slots[self->selected].item;
        short outIds[0x110];
        unsigned char outAmounts[0x110];
        for (int i = 0; i < 0x110; i++)
        {
            outIds[i] = -1;
            outAmounts[i] = 0;
        }
        if (!reorder)
        {
            ListNode0215c530* node = self->head;
            int n = 0;
            while (node)
            {
                for (int i = 0; i < count; i++)
                {
                    if (node->id == ids[i])
                    {
                        outIds[n] = ids[i];
                        outAmounts[n] = amounts[i];
                        n++;
                        node = node->next;
                        break;
                    }
                    if (i == count - 1)
                        node = node->next;
                }
            }
        }
        else
        {
            signed char member = _Z19GetField1c_021a193cPi(func_ov017_0218b5b0()->ptr)->member;
            Item0215c530 item;
            ItemDef0215c530 def;
            _Z18InitStruct020de1d4P11StructDE1D4((StructDE1D4*)&item);
            memset(&def, 0, 0x20);
            item.def = &def;
            int n = 0;
            int rejected = 0;
            for (int i = 0; i < count; i++)
            {
                int id = ids[i];
                if (id > 0)
                {
                    _Z18InitStruct020de1d4P11StructDE1D4((StructDE1D4*)&item);
                    memset(&def, 0, 0x20);
                    Entry0215c530* entry;
                    item.def = &def;
                    entry = _Z22FindEntryById_021db08cPvi((char*)self + 0x19ec, id);
                    if (entry)
                    {
                        item.id = entry->id;
                        item.equipType = entry->equipType;
                        def.a = entry->a;
                        def.b = entry->b;
                        def.c = entry->c;
                        def.d = entry->d;
                        def.e = entry->e;
                        def.f = entry->f;
                        def.g = entry->g;
                        def.h = entry->h;
                    }
                    if (func_020dd4c4(member, &item))
                    {
                        rejected++;
                    }
                    else
                    {
                        outIds[n] = ids[i];
                        outAmounts[n] = amounts[i];
                        n++;
                        ids[i] = -1;
                        amounts[i] = 0;
                    }
                }
            }
            _Z15Compact0207c51cPhi(bag + 0x1d4, data_ov005_0215cd00[self->tab]);
            memcpy(outIds + n, ids, rejected * 2);
            memcpy(outAmounts + n, amounts, rejected);
        }
        memcpy(ids, outIds, count * 2);
        memcpy(amounts, outAmounts, count);

        int sel = self->selected;
        if (sel >= 8 && sel < 0x18)
        {
            int idx = -1;
            for (int i = 0; i < count; i++)
            {
                if (target == ids[i])
                {
                    idx = (short)i;
                    break;
                }
            }
            if (idx >= 0)
            {
                self->prevPage = self->page;
                self->page = idx / 16;
                int old = self->selected;
                self->selected = idx % 16 + 8;
                if (old != self->selected)
                {
                    func_ov005_021579ec(self, 4, (unsigned char)self->selected);
                    func_0205bb04((char*)self + 0x19f4, self->selected - 8);
                }
            }
        }
        _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(data_02108760, 1, 0);
    }

    if (self->modes[self->tab] == 0 || self->modes[self->tab] == 1)
    {
        self->modes[self->tab] = 3;
        self->dirty |= 0x800;
    }
    else if (self->modes[self->tab] == 3)
    {
        self->modes[self->tab] = 2;
        self->dirty |= 0x800;
    }
    else if (self->modes[self->tab] == 2)
    {
        self->modes[self->tab] = 1;
        self->dirty |= 0x800;
    }
    func_ov005_021555c0(self);
    Slot0215c530* slot = &self->slots[self->selected];
    func_ov023_021dcae0(&self->sub.container, slot->item);
    self->dirty |= 0x40;
    ((unsigned char*)self)[0x3daa] = 0;
    func_ov005_0215792c(self, self->f3db9);
}
