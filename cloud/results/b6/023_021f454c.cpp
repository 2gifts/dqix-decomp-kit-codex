#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Grotto/Main/ActiveGrottoClass.h"

struct Random;
struct Obj53dfc;
struct S_10088;
struct Container02070e60 { char storage[4]; };
struct DropDefinition { unsigned short id; unsigned char commonRate, rareRate; unsigned short commonItem, rareItem; };
struct MapItemDetails { unsigned char type, boss; unsigned short isLegacy; };
struct BattleEnemy {
    unsigned short field0 : 3;
    unsigned short defeated : 11;
    unsigned short rareGuaranteed : 1;
    unsigned short commonGuaranteed : 1;
    unsigned short id;
    char pad4[5]; unsigned char previousCount;
};
struct BattleState { char pad0[0x8d66]; BattleEnemy enemies[20]; char pad8e2e[2]; int enemyCount; };
struct BattleJournal { char pad0[0x400]; Container02070e60 monsterTable; };
struct BattleInfo { char pad0[0x24]; unsigned char grottoBoss, legacyBoss; char pad26[0x284-0x26]; BattleJournal journal; };
struct BattleDrop {
    unsigned short item;
    unsigned short enemyId : 15;
    unsigned short field2 : 1;
    unsigned char type;
    signed char owner;
};
struct BattleWork {
    char pad0[0x29c]; BattleState* battle; BattleInfo* info;
    char pad2a4[0x58cc-0x2a4]; unsigned short turns;
    char pad58ce[2]; BattleDrop drops[8]; unsigned char dropCount; signed char memberMask;
    char pad5902[0x6e24-0x5902]; int mapFound; unsigned short mapItem;
    unsigned char mapType, legacyBoss; unsigned short isLegacy;
};
struct Zone { char pad0[0x23ec]; ActiveGrottoClass grotto; };
struct ActorSkills { char pad0[0x150]; unsigned char* skills; };
extern "C" void* func_0202ae18();
extern "C" Zone* func_02012fe4();
int GetCheckedSignedByte(void*, unsigned int);
GameObject* GetCombatantWithFlag0x100(GameState*, int);
int TestBitAt0x34(unsigned char*, unsigned int);
int IsFlag10088Set(S_10088*);
int GetField8FromField19c(Obj53dfc*);
int TestBitInArray0x8ec(unsigned char*, int);
unsigned short GetTableValue(void*);
Random* GetBTRandom();
int NextRandomMax(Random*, int);
DropDefinition* SearchWithLow15Comparator02070fd0(Container02070e60*, int);
extern "C" int func_02032370(int);
extern int data_ov023_021fd888[8];

// USA: func_ov023_021f454c
extern "C" ARM void func_ov023_021f454c(BattleWork* work) {
    GameState* game = GameState::GetInstance();
    BattleJournal* journal = &work->info->journal;
    func_0202ae18();
    DetailedTreasureMapData* map = func_02012fe4()->grotto.GetDetailedData();
    unsigned short bossId = 0;
    if (work->info->grottoBoss || work->info->legacyBoss) {
        if (map->mapType_ == 1) bossId = map->regular_.bossMonsterID_;
        else if (map->mapType_ == 2) bossId = map->legacy_.MaybeGetCurrentAlternateID();
    }
    work->dropCount = 0;
    work->memberMask = 0;
    BattleEnemy* enemies = work->battle->enemies;
    int enemyCount = work->battle->enemyCount;
    BattleDrop* drop = work->drops;
    float turns = work->turns;
    for (int member = 0; member < 5; member++) {
        int rateMultiplier = 100;
        if (member > 0) {
            int id = GetCheckedSignedByte(game, (unsigned char)(member - 1));
            GameObject* actor = GetCombatantWithFlag0x100(game, id);
            if (!actor) continue;
            if (!TestBitAt0x34((unsigned char*)work->info, (unsigned char)id)) continue;
            if (IsFlag10088Set((S_10088*)actor)) continue;
            if ((float)(unsigned int)GetField8FromField19c((Obj53dfc*)actor) / turns < 0.5f) continue;
            if (!TestBitInArray0x8ec(((ActorSkills*)actor)->skills, 0xa4)) continue;
            rateMultiplier = GetTableValue(actor);
            drop->owner = id;
        } else drop->owner = -1;
        if (work->info->grottoBoss || work->info->legacyBoss) {
            int enemyIndex;
            for (enemyIndex = 0; enemyIndex < enemyCount; enemyIndex++) {
                if (enemies[enemyIndex].id == bossId) break;
            }
            BattleEnemy* boss = 0;
            if (enemyIndex < enemyCount) boss = &enemies[enemyIndex];
            Random* random = GetBTRandom();
            unsigned char* dropRates = map->treasureDropRates_;
            for (int treasure = 2; treasure >= 0; treasure--) {
                unsigned int rate = dropRates[treasure];
                if (member > 0) {
                    unsigned int originalRate = rate;
                    rate = (int)(rate * rateMultiplier) / 100;
                    if (originalRate >= 100) rate = 0;
                } else if (boss) {
                    if (boss->rareGuaranteed && treasure == 2) rate = 100;
                    if (boss->commonGuaranteed && treasure == 1) rate = 100;
                }
                if (NextRandomMax(random, 100) < (int)rate) {
                    map->discoveredTreasures_[treasure] = true;
                    MapItemDetails mapDetails;
                    if (GetTreasureMapTypeFromItemID(map->treasureItemIDs_[treasure], (unsigned char*)&mapDetails)) {
                        if (work->mapFound) continue;
                        work->mapFound = 1;
                        work->mapItem = map->treasureItemIDs_[treasure];
                        work->mapType = mapDetails.type;
                        work->legacyBoss = mapDetails.boss;
                        work->isLegacy = mapDetails.isLegacy;
                    } else {
                        drop->item = map->treasureItemIDs_[treasure];
                        drop->enemyId = bossId;
                        DropDefinition* definition = SearchWithLow15Comparator02070fd0(&journal->monsterTable, bossId);
                        if (definition) {
                            if (definition->rareItem == drop->item) drop->type = 2;
                            else if (definition->commonItem == drop->item) drop->type = 1;
                            else drop->type = 0;
                        }
                        work->dropCount++;
                        if (work->dropCount >= 8) break;
                        signed char owner = drop->owner;
                        drop++;
                        drop->owner = owner;
                    }
                }
            }
        }
        if (work->dropCount >= 8) break;
        for (int enemyIndex = 0; enemyIndex < enemyCount; enemyIndex++) {
            BattleEnemy* enemy = &enemies[enemyIndex];
            if (enemy->defeated == enemy->previousCount) continue;
            int enemyId = enemy->id;
            DropDefinition* definition = SearchWithLow15Comparator02070fd0(&journal->monsterTable, enemyId);
            if (!definition) continue;
            if ((work->info->grottoBoss || work->info->legacyBoss) && bossId == enemyId) continue;
            int rareIndex = definition->rareRate;
            int rate = data_ov023_021fd888[rareIndex];
            if (rate > 0) {
                if (member > 0) {
                    rate = rate * 100 / rateMultiplier;
                    if (rareIndex == 0) rate = 0;
                } else if (enemy->rareGuaranteed) rate = 1;
                if (rate > 0 && func_02032370(rate) == 0) {
                    drop->item = definition->rareItem;
                    drop->enemyId = (unsigned short)enemyId;
                    drop->type = 2;
                    work->dropCount++;
                    if (work->dropCount >= 8) break;
                    signed char owner = drop->owner;
                    drop++;
                    drop->owner = owner;
                    continue;
                }
            }
            int commonIndex = definition->commonRate;
            rate = data_ov023_021fd888[commonIndex];
            if (rate > 0) {
                if (member > 0) {
                    rate = rate * 100 / rateMultiplier;
                    if (commonIndex == 0) rate = 0;
                } else if (enemy->commonGuaranteed) rate = 1;
                if (rate > 0 && func_02032370(rate) == 0) {
                    drop->item = definition->commonItem;
                    drop->enemyId = (unsigned short)enemyId;
                    drop->type = 1;
                    work->dropCount++;
                    if (work->dropCount >= 8) break;
                    signed char owner = drop->owner;
                    drop++;
                    drop->owner = owner;
                }
            }
        }
        if (work->dropCount >= 8) break;
    }
    if (work->dropCount || work->mapFound) {
        int member = 0;
        if (member < 4) {
            do {
                if (TestBitAt0x34((unsigned char*)work->info, (unsigned char)member)) {
                    GameObject* actor = GetCombatantWithFlag0x100(game, member);
                    if (actor && (float)(unsigned int)GetField8FromField19c((Obj53dfc*)actor) / turns >= 0.5f)
                        work->memberMask |= 1 << member;
                }
                member++;
            } while (member < 4);
        }
    }
}
