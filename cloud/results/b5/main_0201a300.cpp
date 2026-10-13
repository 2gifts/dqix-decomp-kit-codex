#include <globaldefs.h>

struct GameState;
struct SafeAllocator;
struct TimeStamp020105a8;
struct TileFeaturePlacementData;
struct InitStruct;
struct Array020286fc;
struct Array02028754;
struct Array0202834c;
struct BoundedHalfwordPairList;
struct Container02028170;
struct List_020283c0;
struct AppendPairList;

struct HalfwordPair {
    short a;
    short b;
};

struct Elem02028170 {
    unsigned char id;
    unsigned char unk_1;
    unsigned char unk_2;
    unsigned char flags;
    short z;
    short unk_36;
    short x;
    int unk_3c;
};

struct AppendEntry {
    int a;
    int b;
};

struct Monster0201a300 {
    unsigned short key;
    char pad_2[0xe];
    SafeAllocator* allocator;
    char pad_14[4];
    char list[1];
};

struct Zone0201a300 {
    char pad_0[0x25b4];
    char* mapPtr;
};

extern "C" {
GameState* _ZN9GameState11GetInstanceEv();
void srand(unsigned int seed);
void* memcpy(void* dst, const void* src, unsigned int n);
int abs(int x);
void func_02028214(void* a, int b);
}

void InitializeStruct020280f8(InitStruct* s);
void InitializeStruct(InitStruct* s);
void AllocateAndClearHeaderArray020286fc(Array020286fc* obj, SafeAllocator* alloc, int count);
void AllocateClearArray02028754(Array02028754* obj, SafeAllocator* alloc, int count);
void AllocateArray0202834c(Array0202834c* obj, SafeAllocator* alloc, int count);
void AppendHalfwordPairCapped(BoundedHalfwordPairList* list, HalfwordPair* pair);
void AppendCappedElement02028170(Container02028170* list, Elem02028170* elem);
List_020283c0* FindListEntryById(List_020283c0* list, int id);
void AppendPairToBoundedList(AppendPairList* list, AppendEntry* entry);
void ComputeElapsedTime020105a8(TimeStamp020105a8* obj, int* out);
int GenerateGrottoObjectPositionOrientation(void* zone, int tileX, int tileY, int* outX, int* outZ,
    const TileFeaturePlacementData* tileData, bool preferFaceDown);

struct GrottoWork0201a300 {
    unsigned char cells[256];
};

// USA: func_0201a300
extern "C" ARM int func_0201a300(Zone0201a300* zone, int floor, Monster0201a300* monster,
    const TileFeaturePlacementData* tileData, char** src) {
    GameState* gs = _ZN9GameState11GetInstanceEv();
    GrottoWork0201a300 work;
    Elem02028170 elem;
    HalfwordPair pair;
    AppendEntry e;
    int outZ;
    int outX;
    int elapsed;
    if (monster == 0) {
        return 1;
    }
    srand(monster->key % 20);
    memcpy(work.cells, *(src == 0 ? &zone->mapPtr : src), 0x100);
    int open = 0;
    int edges = 0;
    for (int y = 1; y < 15; y++) {
        for (int x = 1; x < 15; x++) {
            int t = work.cells[x + y * 16];
            if (t == 1 || t == 3) {
                continue;
            }
            int l = work.cells[x - 1 + y * 16];
            int u = work.cells[x + (y - 1) * 16];
            if (l != 1 && l != 3) {
                edges++;
            }
            open++;
            if (u != 1 && u != 3) {
                edges++;
            }
        }
    }
    int d = 0x1020 - (open * 16 + edges * 8);
    if (d < 0) {
        edges -= abs(d / 8) + 1;
    }
    InitializeStruct020280f8((InitStruct*)monster->list);
    InitializeStruct((InitStruct*)monster->list);
    AllocateAndClearHeaderArray020286fc((Array020286fc*)monster->list, monster->allocator, open);
    AllocateClearArray02028754((Array02028754*)monster->list, monster->allocator, edges);
    AllocateArray0202834c((Array0202834c*)monster->list, monster->allocator, 1);
    pair.a = 0;
    pair.b = 0;
    AppendHalfwordPairCapped((BoundedHalfwordPairList*)monster->list, &pair);
    for (int y = 1; y < 15; y++) {
        for (int x = 1; x < 15; x++) {
            int idx = x + y * 16;
            int t = work.cells[x + y * 16];
            if (t == 1 || t == 3) {
                continue;
            }
            elem.id = 0;
            elem.unk_1 = 0;
            elem.unk_2 = 0;
            elem.unk_3c = 0;
            elem.flags = 0;
            GenerateGrottoObjectPositionOrientation(zone, x, y, &outZ, &outX, tileData, false);
            elem.unk_36 = 0;
            elem.z = outZ >> 12;
            elem.x = outX >> 12;
            elem.id = idx;
            if (t == 4) {
                elem.flags |= 2;
            }
            AppendCappedElement02028170((Container02028170*)monster->list, &elem);
            int left = work.cells[x - 1 + y * 16];
            int up = work.cells[x + (y - 1) * 16];
            if (left != 1 && left != 3) {
                e.b = 0;
                e.a = 0;
                e.a = (int)FindListEntryById((List_020283c0*)monster->list, y * 16 + x);
                e.b = (int)FindListEntryById((List_020283c0*)monster->list, x - 1 + y * 16);
                AppendPairToBoundedList((AppendPairList*)monster->list, &e);
            }
            if (up != 1 && up != 3) {
                e.b = 0;
                e.a = 0;
                e.a = (int)FindListEntryById((List_020283c0*)monster->list, idx);
                e.b = (int)FindListEntryById((List_020283c0*)monster->list, x + (y - 1) * 16);
                AppendPairToBoundedList((AppendPairList*)monster->list, &e);
            }
        }
    }
    func_02028214(monster->list, *(int*)((char*)monster + 0x10));
    ComputeElapsedTime020105a8((TimeStamp020105a8*)gs, &elapsed);
    srand(elapsed);
    return 1;
}
