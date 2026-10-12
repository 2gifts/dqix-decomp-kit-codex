#include <globaldefs.h>
#include "GameState/GameState.h"

struct Grid021f9b30 {
    char pad0[0x5c];
    short page;
    short field5e;
};

struct Node02165ea4 {
    char pad0[0x3a];
    unsigned char hidden;
    char pad3b[9];
    short field44;
    short field46;
};

struct GetTableByteOrFFData {
    char pad0[0xa8];
    unsigned char bits[1];
};

struct Entry02166bd8 {
    short key;
    unsigned short state : 2;
    unsigned short flag2 : 1;
    unsigned short flag3 : 1;
    unsigned short skipNode : 1;
    unsigned short reserved : 11;
    char name[4];
    Entry02166bd8* next;
};

struct Found0208d994 {
    int field0;
    const char* text;
};

struct List0208d994 {
    int pad[4];
};

struct Lists02166bd8 {
    char pad0[8];
    Entry02166bd8* head;
    List0208d994 list;
};

struct Globals0217101c {
    Lists02166bd8* lists;
    char** buffers;
};

extern "C" void* func_0205ec34();
void DispatchStateTransitions_02166608(void*);
extern "C" Grid021f9b30* func_ov023_021f6524(void*, int);
GetTableByteOrFFData* GetGlobal02109418();
void GetGlobalField0x1c020421a0();
extern "C" void func_ov023_021f65d4(void*, int, int);
Found0208d994* FindElementByKey0208d994(List0208d994*, int);
extern "C" void _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(Grid021f9b30*, unsigned short, unsigned int, unsigned int);
Node02165ea4* SetFieldIfType1_02165ea4_02165ea4(void*, int, int);
extern "C" int _Z24NormalizeField5_0200fb08P14Struct0200fb08(GameState*);
unsigned int GetTableByteOrFF(GetTableByteOrFFData*, unsigned int);
extern "C" int _Z26TestBitInByteArray02096100iPhi(GetTableByteOrFFData*, unsigned char*, int);
const char* Lookup4Entry_021f63ac(void*, int, int);
extern "C" void* memset(void*, int, unsigned int);
void AppendXTag(char*, int);
void AppendString02042058(char*, const char*);
extern "C" void func_ov023_021f63f8(void*, int, char*, int);
void ClearNodeMaskById_021f6600(void*, int, int);
extern "C" void func_ov023_021f9d14(Grid021f9b30*, void*);
void DispatchNode_021f6630(int, int, short, short);

extern Globals0217101c data_ov004_0217101c;

// USA: func_ov004_02166bd8
extern "C" ARM int func_ov004_02166bd8(void* a) {
    func_0205ec34();
    DispatchStateTransitions_02166608(a);
    short page = 0;
    short field5e = 1;
    Grid021f9b30* grid = func_ov023_021f6524(a, 0x5b);
    if (grid) {
        page = grid->page;
        field5e = grid->field5e;
    }
    Entry02166bd8* entry;
    int idA = 0xbe;
    int idB = 0xc8;
    int idC = 0xd2;
    int idD = 0xdc;
    Lists02166bd8* lists = data_ov004_0217101c.lists;
    entry = lists->head;
    short skip = page * 8;
    while (entry && skip) {
        skip--;
        entry = entry->next;
    }
    GetTableByteOrFFData* table = GetGlobal02109418();
    GetGlobalField0x1c020421a0();
    unsigned char* bits = table->bits;
    for (short i = 0; i < 8; idA++, idB++, idC++, idD++, i++) {
        int shown = 0;
        func_ov023_021f65d4(a, idA, 8);
        func_ov023_021f65d4(a, idB, 8);
        func_ov023_021f65d4(a, idC, 8);
        func_ov023_021f65d4(a, idD, 8);
        if (entry) {
            Found0208d994* found = FindElementByKey0208d994(&lists->list, entry->key);
            if (found) {
                shown = 1;
                _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(grid, idA, (unsigned short)i, 0);
                int extra = 0;
                if (!entry->skipNode) {
                    Node02165ea4* node = SetFieldIfType1_02165ea4_02165ea4(a, idD, 6);
                    node->field44 = 4;
                    node->field46 = 0;
                    node->hidden = (i != 0);
                    if (i >= 1 && i <= 3) {
                        int lang = _Z24NormalizeField5_0200fb08P14Struct0200fb08(GameState::GetInstance());
                        int mode = 2;
                        switch (lang) {
                        case 4:
                            mode = 3;
                            break;
                        case 3:
                            mode = 1;
                            break;
                        case 5:
                            mode = 3;
                            break;
                        }
                        node->field46 = mode;
                    }
                    extra = 1;
                }
                unsigned int id = GetTableByteOrFF(table, (unsigned char)entry->key);
                int seen = _Z26TestBitInByteArray02096100iPhi(table, bits + 0x400, id);
                int type = 10;
                const char* text = found->text;
                switch (entry->state) {
                case 0:
                case 1:
                    if (!seen)
                        text = Lookup4Entry_021f63ac(a, 3, 10);
                    break;
                case 2:
                    type = 11;
                    break;
                case 3:
                    type = 12;
                    break;
                }
                char* buf = data_ov004_0217101c.buffers[i];
                memset(buf, 0, 0x80);
                AppendXTag(buf, 0x1c);
                AppendString02042058(buf, entry->name);
                AppendXTag(buf, 0x2e);
                if (text)
                    AppendString02042058(buf, text);
                func_ov023_021f63f8(a, idA, buf, 0xf);
                SetFieldIfType1_02165ea4_02165ea4(a, idC, type)->hidden = 0;
                ClearNodeMaskById_021f6600(a, idA, 8);
                ClearNodeMaskById_021f6600(a, idC, 8);
                if (extra)
                    ClearNodeMaskById_021f6600(a, idD, 8);
            }
            entry = entry->next;
        }
        if (!shown)
            _Z26SetCellIfInBounds_021f9b30P12Grid021f9b30sjj(grid, 0, (unsigned short)i, 0);
    }
    func_ov023_021f9d14(grid, a);
    DispatchNode_021f6630((int)a, 0x17, page, field5e);
    return 0;
}
