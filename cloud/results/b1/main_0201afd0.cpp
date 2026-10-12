#include <globaldefs.h>
#include "GameState/GameState.h"
#include "World/ZoneFeatures.h"
#include "World/LootableContainer.h"
#include "Filesystem/FileIO.h"
#include "Filesystem/BackgroundLoader.h"
#include "System/Memory.h"
#include "std_library_functions.h"

struct TilePoint0201afd0 {
    unsigned char x;
    unsigned char y;
};

struct AdjacencyBits0201afd0 {
    unsigned char bits;
};

struct FloorMap0201afd0 {
    unsigned char* mapData;
    unsigned char* adjacencyData;
    int width;
    int height;
    TilePoint0201afd0 stairsUp;
    TilePoint0201afd0 stairsDown;
    char pad_14[0x7c - 0x14];
    TilePoint0201afd0 chests[5];
    char pad_86[2];
    int chestCount;
};

struct ActiveGrotto0201afd0 {
    char pad_0[0x1c8];
    FloorMap0201afd0 floorMap;
    char pad_254[0x278 - 0x254];
};

struct ChestRecord0201afd0 {
    Vector3i position;
    short angleY;
    short angleX;
    short unk_10;
    char unk_12[4];
    unsigned char state;
    unsigned char kind;
    unsigned char searched;
    char unk_19;
    short chestIndex;
    unsigned short lootIndex;
    char unk_1e[6];
};

struct ChestLoot0201afd0 {
    unsigned short uniqueID;
    unsigned short itemIDOrRank;
    unsigned short unk_4_0 : 2;
    unsigned short lootType : 2;
    unsigned short containerType : 3;
    short unk_6;
    Vector3i position;
};

struct ChestLootList0201afd0 {
    ChestLoot0201afd0* entries;
    int count;
    int capacity;
};

struct Zone0201afd0 {
    char pad_0[0x68];
    SafeAllocator* allocator;
    ZoneFeatures features;
    char pad_f4[0x477 - 0xf4];
    unsigned char numChests;
    char pad_478[4];
    ChestRecord0201afd0* chests;
    char pad_480[0x23ba - 0x480];
    signed char currentGrottoFloor;
    char pad_23bb[0x23ec - 0x23bb];
    ActiveGrotto0201afd0 grotto;
    char pad_2664[0x2724 - 0x2664];
    ChestLootList0201afd0 chestLoot;
    char pad_2730[0x2754 - 0x2730];
    char itemNames[0x18];
};

struct NarcName0201afd0 {
    char name[11];
};

extern "C" {
void* func_0202ae18();
void _Z26ComputeElapsedTime020105a8P17TimeStamp020105a8Pi(GameState* gs, int* out);
unsigned short _ZNK17ActiveGrottoClass19GetActiveGrottoSeedEv(const ActiveGrotto0201afd0* grotto);
int _ZNK17ActiveGrottoClass22GetActiveGrottoEnvironEv(const ActiveGrotto0201afd0* grotto);
int _ZN17ActiveGrottoClass18RandomizeChestRankEi(ActiveGrotto0201afd0* grotto, int floor);
int _ZNK8FloorMap16GetAdjacencyBitsEii(const FloorMap0201afd0* map, int x, int y);
int _Z39GenerateGrottoObjectPositionOrientationPviiPiS0_PK24TileFeaturePlacementDatab(
    void* zone, int tileX, int tileY, int* outX, int* outZ, const TileFeaturePlacementData* tileData,
    bool preferFaceDown);
ZoneFeatures::Opcode6aEntry* _Z13FindEntryByIdP15EntryList_1e690i(ZoneFeatures* features, int id);
void _Z23ClearThreeWords02094d00P29ClearThreeWords02094d00Struct(ChestLootList0201afd0* list);
void _Z21AllocateArray02094cdcP17AllocDest02094cdciP13SafeAllocator(
    ChestLootList0201afd0* list, int count, SafeAllocator* alloc);
void _Z18InitStruct020133fcPc(ChestRecord0201afd0* record);
int _Z10GetBit5cd0P10S_02011430ii(GameState* gs, int floor, int index);
int _Z18CheckField0NonZeroPi(void* search);
void _Z15CopyElemToIndexP17Container02094d14P12Elem02094d14i(
    ChestLootList0201afd0* list, ChestLoot0201afd0* loot, int index);
void _Z16ZeroInit020de848Pv(void* names);
void* ExtractFileFromGP2(const char* gp2Path, const char* innerFilePath, unsigned int* outSize);
int func_020de9a4(void* names, SafeAllocator* alloc, void* data, unsigned int size,
    unsigned short* itemIDs, unsigned short count);
void __clear(void* buf, int size);
}

extern const unsigned char data_020e6e44[15];
extern const NarcName0201afd0 data_020e6df0;
extern const char* data_020f2a38;
extern const char* data_020f2a30;

static inline TilePoint0201afd0 GetChestTile0201afd0(const FloorMap0201afd0* map, unsigned char index) {
    return map->chests[index];
}

static inline AdjacencyBits0201afd0 GetAdjacency0201afd0(const FloorMap0201afd0* map, int x, int y) {
    AdjacencyBits0201afd0 result;
    result.bits = _ZNK8FloorMap16GetAdjacencyBitsEii(map, x, y);
    return result;
}

static inline int FxSquare0201afd0(int a) {
    return (int)(((long long)a * a + 0x800) >> 12);
}

static inline int GetFloorSeed0201afd0(const ActiveGrotto0201afd0* grotto, int floor) {
    unsigned short seed = _ZNK17ActiveGrottoClass19GetActiveGrottoSeedEv(grotto);
    return seed + floor;
}

// USA: func_0201afd0
extern "C" ARM int func_0201afd0(Zone0201afd0* self, const void* treasureArchive) {
    GameState* gs = GameState::GetInstance();
    ActiveGrotto0201afd0* grotto = &self->grotto;
    int i;
    FloorMap0201afd0* map = &grotto->floorMap;
    void* search = func_0202ae18();
    ChestLootList0201afd0* lootList = &self->chestLoot;

    self->numChests = map->chestCount;
    if (self->numChests == 0) {
        int elapsed;

        _Z26ComputeElapsedTime020105a8P17TimeStamp020105a8Pi(gs, &elapsed);
        srand(elapsed + GetFloorSeed0201afd0(grotto, self->currentGrottoFloor));
        return 0;
    }

    self->chests = (ChestRecord0201afd0*)self->allocator->Allocate(self->numChests * sizeof(ChestRecord0201afd0));
    self->features.AllocateOpcode6aEntries(self->numChests, self->allocator);
    srand(GetFloorSeed0201afd0(grotto, self->currentGrottoFloor));

    for (i = 0; i < self->numChests; i++) {
        ZoneFeatures::Opcode6aEntry entry;
        TilePoint0201afd0 tile;
        AdjacencyBits0201afd0 adjacency;
        int dir;
        int found;

        VectorizedMemset(&entry, 0, sizeof(entry));
        entry.unk_20 = 0;
        entry.unk_22 = 0;
        entry.unk_24 = 0;
        entry.unk_8.vec.x = 0;
        entry.unk_8.vec.y = 0;
        entry.unk_8.vec.z = 0;
        entry.unk_28 = 0;
        entry.pNext = NULL;

        entry.unk_0 = i;
        entry.unk_14.vec.z = 0x1000;
        entry.unk_14.vec.y = 0x1000;
        entry.unk_14.vec.x = 0x1000;
        entry.maybeType = 1;

        tile = GetChestTile0201afd0(map, i);
        dir = _Z39GenerateGrottoObjectPositionOrientationPviiPiS0_PK24TileFeaturePlacementDatab(
            self, tile.x, tile.y, &entry.unk_8.vec.x, &entry.unk_8.vec.z, NULL, false);
        adjacency = GetAdjacency0201afd0(map, tile.x, tile.y);
        found = 0;
        if (_ZNK17ActiveGrottoClass22GetActiveGrottoEnvironEv(grotto) == 2) {
            for (int j = 0; j < 15; j++) {
                if (adjacency.bits == data_020e6e44[j]) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                entry.unk_8.vec.y = -0x4cc;
            }
        }

        switch (dir) {
        case 0:
            entry.unk_20 = entry.unk_22 = 0x3244;
            break;
        case 1:
            entry.unk_20 = entry.unk_22 = 0x1922;
            break;
        case 2:
            entry.unk_20 = entry.unk_22 = 0;
            break;
        case 3:
            entry.unk_20 = entry.unk_22 = 0x4b66;
            break;
        }

        {
            fix32_t halfX = entry.unk_14.vec.x / 2;
            fix32_t halfZ = entry.unk_14.vec.z / 2;
            fix32_t zSq = FxSquare0201afd0(halfZ);
            fix32_t xSq = FxSquare0201afd0(halfX);

            entry.unk_24 = xSq + zSq;
        }
        entry.pNext = NULL;
        self->features.CreateOpcode6aEntry(entry);
    }

    {
        SafeAllocator alloc;
        char buffer[0x400];
        LootDistribution distribution;
        NarcName0201afd0 name;
        const void* script;
        unsigned int scriptLength;
        int ranks[32];
        unsigned short itemIDs[100];
        int elapsed;
        int itemCount;
        int chest;
        int index;

        alloc.ResetAllocatorPointer();
        alloc.CreateTypeA(buffer, sizeof(buffer));
        name = data_020e6df0;
        if (GetFileInNarcPermissive(treasureArchive, name.name, &script, &scriptLength)) {
            distribution.Reset();
            distribution.LoadFromScript(&alloc, script, scriptLength);
        }

        _Z23ClearThreeWords02094d00P29ClearThreeWords02094d00Struct(lootList);
        _Z21AllocateArray02094cdcP17AllocDest02094cdciP13SafeAllocator(lootList, self->numChests, self->allocator);
        memset(ranks, 1, sizeof(ranks));
        for (chest = 0; chest < self->numChests; chest++) {
            ranks[chest] = _ZN17ActiveGrottoClass18RandomizeChestRankEi(grotto, self->currentGrottoFloor);
        }

        _Z26ComputeElapsedTime020105a8P17TimeStamp020105a8Pi(gs, &elapsed);
        srand(elapsed + GetFloorSeed0201afd0(grotto, self->currentGrottoFloor));

        __clear(itemIDs, sizeof(itemIDs));
        itemCount = 0;
        for (index = 0; index < self->numChests; index++) {
            ZoneFeatures::Opcode6aEntry* entry = _Z13FindEntryByIdP15EntryList_1e690i(&self->features, index);
            const LootDistribution::Outcome* outcome = distribution.Sample(ranks[index]);

            if (outcome != NULL) {
                ChestRecord0201afd0* record = &self->chests[index];
                ChestLoot0201afd0 loot;

                _Z18InitStruct020133fcPc(record);
                record->position = entry->unk_8.vec;
                record->angleY = entry->unk_20;
                record->unk_10 = 0;
                record->chestIndex = index;
                record->lootIndex = index;
                record->kind = 2;
                if (_Z10GetBit5cd0P10S_02011430ii(gs, self->currentGrottoFloor, index)) {
                    record->state = 8;
                    record->angleX = 0x4000;
                } else {
                    record->state = 1;
                    record->angleX = 0x647a;
                }
                if (_Z18CheckField0NonZeroPi(search)) {
                    record->searched = 1;
                }
                if (outcome->lootType == 2) {
                    itemIDs[itemCount++] = outcome->itemID;
                }

                loot.uniqueID = 0;
                loot.itemIDOrRank = outcome->itemID;
                loot.unk_4_0 = 0;
                loot.lootType = outcome->lootType;
                loot.containerType = 4;
                loot.position = entry->unk_8.vec;
                _Z15CopyElemToIndexP17Container02094d14P12Elem02094d14i(lootList, &loot, record->lootIndex);
            }
        }

        _Z16ZeroInit020de848Pv(self->itemNames);
        if (itemCount > 0) {
            BackgroundLoader::AddLockGlobal();
            unsigned int size = 0;
            void* data = ExtractFileFromGP2(data_020f2a38, data_020f2a30, &size);

            if (data != NULL) {
                func_020de9a4(self->itemNames, self->allocator, data, size, itemIDs, itemCount);
            }
            BackgroundLoader::RemoveLockGlobal();
        }
        alloc.Reset();
        alloc.Destroy();
    }
    return 1;
}
