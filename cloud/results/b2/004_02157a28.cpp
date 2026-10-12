#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"

struct Container020dedd0;
struct Field150Holder02052df8;
struct KeyedList0207c484;
struct HeadNode02046b24;
struct Ctx021a3498;

struct ElementBits_02157a28 {
    unsigned int category : 4;
    unsigned int unknown : 14;
    unsigned int flag : 1;
    unsigned int rest : 13;
};
struct Element_02157a28 {
    char pad0[8];
    ElementBits_02157a28 bits;
};
struct BattleBlk_02157a28 {
    char pad0[0xe];
    short key;
    char pad10;
    unsigned char count;
    char pad12[0x28];
    short extra;
    char pad3c[0xb];
    unsigned char used;
    char pad48[0x55];
    unsigned char dirty;
};
struct Data021707d8_02157a28 { char pad[8]; BattleBlk_02157a28* ptr; };
extern Data021707d8_02157a28 data_ov004_021707d8;

struct SlotTable_02157a28 { int v[8]; };
extern SlotTable_02157a28 data_ov004_0216fb90;

struct TextContext_02157a28 {
    SafeAllocator allocator;
    char list[0x18];
    Container020dedd0* containerData;
    int field30;
    int field34;
};

extern "C" short func_ov004_02157128(void*);
extern "C" int* func_0202ae18();
extern "C" void* func_ov004_02156fd4(void*, int);
extern "C" void func_0207cc18(void*, Element_02157a28*);
extern "C" void func_02052d7c(void*, int, short);
extern "C" void func_0207c378(char*, int, int, int);
extern "C" void func_ov017_021c3fb4(int, int);
extern "C" void func_ov017_021c4418(int, int);
extern "C" void func_ov017_021c9e00(int, int, int, int);
extern "C" GameResources* func_ov017_0218b5b0();
extern "C" void func_ov017_0218f5a4(GameResources*, int, int, int, int);
extern "C" void func_ov011_021848a0(void*, int);

Element_02157a28* FindElementByKey020dedd0(Container020dedd0*, int);
void* ZeroInitReturn020de824(void*);
void InitStruct0207cbe8(char*);
void InitStruct0207cc0c(char*);
void* GetPtrField0x2a04(GameState*);
int GetHalfwordEntryFromField150(Field150Holder02052df8*, int);
void DecrementKeyedStackAmount0207c484(KeyedList0207c484*, int, int, int);
void TrySetFlags0x130(unsigned char*, unsigned short);
int CheckField0NonZero(int*);
int GetHeadNodeIdOrMinusOne(HeadNode02046b24**);
void ClearIfMatchAndFinalize_021a3498(Ctx021a3498*);

// USA: func_ov004_02157a28
extern "C" ARM int func_ov004_02157a28(void* a, int flag) {
    short id = func_ov004_02157128(a);
    int* search = func_0202ae18();
    GameState* gs = GameState::GetInstance();
    GameObject* c = GetCombatantWithFlag0x100(gs, id);
    if (!c) return 0;
    Container020dedd0* cont = (Container020dedd0*)func_ov004_02156fd4(a, 5);
    if (!cont) return 0;
    Element_02157a28* elem = FindElementByKey020dedd0(cont, data_ov004_021707d8.ptr->key);
    if (!elem) return 0;
    int category = elem->bits.category;
    TextContext_02157a28 context;
    context.allocator.ResetAllocatorPointer();
    ZeroInitReturn020de824(context.list);
    InitStruct0207cbe8((char*)&context);
    InitStruct0207cbe8((char*)&context);
    context.containerData = cont;
    func_0207cc18(&context, elem);
    InitStruct0207cc0c((char*)&context);

    int found = 0;
    int i;
    for (i = 0; i != 8; i++) {
        if (i == category) {
            found = 1;
            break;
        }
    }
    if (!found) return 0;
    SlotTable_02157a28 table = data_ov004_0216fb90;
    int slot = table.v[i];
    if (slot < 0) return 0;

    char* bag = (char*)GetPtrField0x2a04(gs);
    int equipped = GetHalfwordEntryFromField150((Field150Holder02052df8*)c, slot);
    int same = data_ov004_021707d8.ptr->key == equipped;
    func_02052d7c(c, slot, data_ov004_021707d8.ptr->key);
    if (data_ov004_021707d8.ptr->used == 0) {
        func_0207c378(bag + 0x1d4, data_ov004_021707d8.ptr->key, (signed char)(data_ov004_021707d8.ptr->count - 1), category);
        data_ov004_021707d8.ptr->dirty = 0;
    } else {
        DecrementKeyedStackAmount0207c484((KeyedList0207c484*)(bag + 0x1d4), data_ov004_021707d8.ptr->key, 1, category);
    }
    data_ov004_021707d8.ptr->used++;
    if (FindElementByKey020dedd0(cont, equipped)) {
        func_0207c378(bag + 0x1d4, equipped, 1, category);
    }
    int upgrade = 0;
    BattleBlk_02157a28* blk = data_ov004_021707d8.ptr;
    unsigned char level = blk->extra;
    if (blk->used < blk->count) {
        if (level >= 2) upgrade = 1;
    }
    if (!same || flag) {
        int did = 0;
        Element_02157a28* e2 = FindElementByKey020dedd0(cont, data_ov004_021707d8.ptr->key);
        if (e2 && e2->bits.flag) {
            TrySetFlags0x130((unsigned char*)c, 4);
            did = 1;
        }
        if (CheckField0NonZero(search)) {
            func_ov017_021c3fb4(id, 1);
            func_ov017_021c4418(id, -1);
            if (did) func_ov017_021c9e00(id, 0, 0, 1);
        }
        GameResources* res = func_ov017_0218b5b0();
        HeadNode02046b24** headA = (HeadNode02046b24**)res->unknown_ptr_array_36fc[1];
        Ctx021a3498* ctxB = (Ctx021a3498*)res->unknown_ptr_array_36fc[2];
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        func_ov017_0218f5a4(res, id, 1, 0, 0);
        while (GetHeadNodeIdOrMinusOne(headA) == 0x13) {
            loader->RemoveAllLocks();
            ClearIfMatchAndFinalize_021a3498((Ctx021a3498*)headA);
            if (GetHeadNodeIdOrMinusOne((HeadNode02046b24**)ctxB) == 0x14) {
                ClearIfMatchAndFinalize_021a3498(ctxB);
            }
        }
        if (did) {
            if (upgrade) func_ov011_021848a0(a, 0x3e9);
            else func_ov011_021848a0(a, 0x3e8);
            return 0;
        }
    }
    if (upgrade) func_ov011_021848a0(a, 0x23bf);
    else func_ov011_021848a0(a, 0x393);
    return 0;
}
