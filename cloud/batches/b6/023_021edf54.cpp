#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Combat/Main/BattleList.h"
#include "Filesystem/BackgroundLoader.h"

template <typename T, int N> struct StatArray { T values_[N]; };
struct CharacterStats {
    StatArray<int, 6> field0_; StatArray<int, 9> field18_; StatArray<int, 22> field3c_; StatArray<int, 41> field94_;
    StatArray<unsigned int, 13> experience_; StatArray<unsigned short, 13> levels_; StatArray<unsigned char, 13> revocations_; char pad193;
    StatArray<int, 88> field194_, field2f4_;
    StatArray<unsigned short, 8> field454_; StatArray<unsigned char, 27> field464_; char pad47f;
    StatArray<int, 2> field480_; StatArray<unsigned short, 14> field488_; StatArray<unsigned short, 96> field4a4_;
    unsigned short points_, field566_; short field568_;
    unsigned char field56a_, field56b_, field56c_, field56d_, field56e_, field56f_;
    StatArray<unsigned char, 384> field570_; StatArray<int, 88> field6f0_; StatArray<int, 39> field850_;
    StatArray<unsigned char, 36> field8ec_; StatArray<unsigned char, 9> field910_; char pad919[3];
    StatArray<int, 6> field91c_; StatArray<unsigned char, 24> field934_;
    int field94c_, vocation_; unsigned short field954_;
    short field956_, field958_, field95a_, field95c_, field95e_;
    unsigned short field960_, field962_;
};
struct MemberResult {
    unsigned char member_, vocation_, level_, blocked_; unsigned int experience_;
    char name_[48]; CharacterStats stats_;
};
struct BattleEndState {
    char pad0[0x10]; unsigned char nothing_;
    char pad11[0xf4 - 0x11]; char waitState_[0x118 - 0xf4];
    MemberResult members_[4]; unsigned char memberCount_, sequence_[4], sequenceLength_;
};
struct BattleGroup {
    unsigned short flags_ : 3; unsigned short count_ : 11; unsigned short reserved_ : 2;
    unsigned short monster_; short record_; unsigned char flag_; char pad7[3];
};
struct EncounterInfo {
    unsigned char count_; unsigned char flags_ : 6; unsigned char kind_ : 2;
    char pad2[0x4e - 2]; unsigned short monster_;
};
struct BattleWork {
    char pad0[0x81b0]; EncounterInfo encounter_;
    char pad8200[0x8d66 - 0x8200]; BattleGroup groups_[16];
    char pad8e06[0x8e15 - 0x8e06]; unsigned char defeated_;
    char pad8e16[0x8e20 - 0x8e16]; int turns_, field8e24_, experience_, gold_, groupCount_;
    unsigned int bonusExperience_; int field8e38_; float experienceMultiplier_, goldMultiplier_;
};
struct BattleInfo {
    char pad0[8]; unsigned short id_; char pada[2]; int kind_;
    char pad10[0x25 - 0x10]; unsigned char deferred_; char pad26[4]; signed char leader_;
};
struct BattleResultScene {
    char pad0[0x29c]; BattleWork* work_; BattleInfo* info_;
    char pad2a4[0xeac - 0x2a4]; int endState_;
    char padeb0[0x5758 - 0xeb0]; unsigned int experience_[4]; int gold_;
    char pad576c[0x58c0 - 0x576c]; unsigned int totalExperience_, totalGold_;
    unsigned char members_, resultKind_; unsigned short monster_, turns_; char pad58ce[2];
    char drops_[0x30]; unsigned char dropCount_; signed char dropMembers_;
    char pad5902[0x6e24 - 0x5902]; int map_; unsigned short mapItem_;
};
struct ResultActor { char pad0[0x134]; char* name_; char pad138[0x150 - 0x138]; CharacterStats* stats_; };
struct StoreStruct { char pad0[0x19b2]; unsigned char flag19b2_; };
struct Flags_021ed7e0 { unsigned int count_ : 10; unsigned int flag_ : 1; unsigned int rest_ : 21; };
struct S_a0858 { char bytes_[0x68]; };
struct S_a0300 { char bytes_[0x48]; };
struct BattleBlock { S_a0858 battle_; S_a0300 total_; };
struct SearchStruct0202c1a4;
struct S_10088;
struct Obj021eea98;
struct TagValueEntry020e385c;
extern "C" BattleEndState* _ZZ17GetGlobal021ffefcvE1s;
StoreStruct* GetGlobalField0x1c020421a0();
void ClearSubstructFlag0x4(unsigned char*);
void SetFieldForListedCombatants021f50fc();
int CheckField0NonZero(int*);
int GetSearchStructCurrentArrEntry(SearchStruct0202c1a4*);
int TestBitAt0x34(unsigned char*, unsigned int);
void EnqueueEventTag105_021cd4c8(unsigned short, unsigned short, unsigned char);
int GetField0x3acValue(GameState*);
void LoadBattleBlock020ac4c0(void*);
void AddSaturateField0x48(S_a0858*, unsigned int);
void AddClamped24BitFieldAt0x24(S_a0300*, unsigned int);
void CopyInBattleField0x7540(void*);
void SetCombatWorkFlags0x55f4(void*, int);
int GetCombatWorkFlags0x55f4(void*, int);
void ClearFlagBits10to24_021ed7e0(Flags_021ed7e0*);
int CopyIndexedRegion0x75f0(void*, short*, int*, int);
int GetTableValue(void*);
int IsFlag10088Set(S_10088*);
unsigned int GetTableEntry138_021eea98(Obj021eea98*);
CharacterStats* GetFieldAt0x150(unsigned char*);
int CopyOutRegion0x571d(char*, void*);
TagValueEntry020e385c* GetData02153660();
void SetEntryBitUnlessMatching020e39d4(TagValueEntry020e385c*, int, int, int);
extern "C" {
int func_ov023_021f5340(void*);
void* func_0202ae18();
int func_ov000_0215e9fc(BattleWork*, short*, int, int);
int func_ov000_0215ec1c(BattleWork*, short*, int, int);
unsigned int func_ov023_021f4098(BattleResultScene*, int, unsigned int);
void func_ov023_021f454c(BattleResultScene*);
void func_ov017_021cd218(unsigned short, unsigned short, unsigned short, unsigned char, unsigned char, unsigned short);
void func_ov017_021cd590(unsigned short, unsigned int*, unsigned char, int);
void func_ov017_021cd35c(unsigned short, void*, unsigned char, signed char);
int func_0202c540(void*);
void func_020abe84(void*, short*, Flags_021ed7e0*, int);
int func_ov023_021f5150(BattleResultScene*, BattleInfo*, unsigned int*, int*, int);
}

// USA: func_ov023_021edf54
extern "C" ARM int func_ov023_021edf54(BattleResultScene* self) {
    GameState* game = GameState::GetInstance();
    if (!func_ov023_021f5340(_ZZ17GetGlobal021ffefcvE1s->waitState_)) return self->endState_;
    if (BackgroundLoader::GetInstance()->GetNumQueuedTasks() > 0) return self->endState_;
    GameResources* resources = func_ov017_0218b5b0();
    StoreStruct* messages = GetGlobalField0x1c020421a0();
    void* search = func_0202ae18();
    BattleWork* work = self->work_;
    if (work) {
        GameState* actors = GameState::GetInstance();
        short members[16];
        int count = 0;
        count += func_ov000_0215e9fc(work, members, 16, 0);
        count += func_ov000_0215ec1c(work, members + count, 16 - count, 0);
        for (int i = 0; i < count; ++i) {
            GameObject* actor = actors->GetCombatantByIndex(members[i]);
            if (actor) ClearSubstructFlag0x4((unsigned char*)actor);
        }
    }
    SetFieldForListedCombatants021f50fc();
    if (!CheckField0NonZero((int*)search) || self->info_->leader_ == GetSearchStructCurrentArrEntry((SearchStruct0202c1a4*)search)) {
        int experience = self->work_->experience_;
        float experienceMultiplier = self->work_->experienceMultiplier_;
        self->totalExperience_ = (unsigned int)((float)experience * experienceMultiplier);
        int gold = self->work_->gold_;
        float goldMultiplier = self->work_->goldMultiplier_;
        self->totalGold_ = (unsigned int)((float)gold * goldMultiplier);
        self->turns_ = self->work_->turns_;
        short members[4];
        int count = func_ov000_0215e9fc(self->work_, members, 4, 1);
        self->members_ = 0;
        for (int i = 0; i < count; ++i) self->members_ |= 1 << members[i];
        for (int i = 0; i < 4; ++i)
            if (TestBitAt0x34((unsigned char*)self->info_, (unsigned char)i)) self->experience_[i] = func_ov023_021f4098(self, i, 0);
        int groupCount = self->work_->groupCount_;
        BattleGroup* groups = self->work_->groups_;
        if (!groupCount) {
            EncounterInfo* encounter = &self->work_->encounter_;
            int defeated = 0;
            if (encounter->count_ <= self->work_->defeated_) defeated = 1;
            if (encounter->kind_ > 1) {
                self->resultKind_ = !defeated ? 5 : 8;
                self->monster_ = 0;
            } else if (encounter->count_ > 1) {
                self->resultKind_ = !defeated ? 4 : 7;
                self->monster_ = encounter->monster_;
            } else {
                self->resultKind_ = !defeated ? 3 : 6;
                self->monster_ = encounter->monster_;
            }
        } else {
            int total = 0, unique = 0;
            int monsters[8];
            for (int i = 0; i < groupCount; ++i) {
                total += groups[i].count_;
                int j = 0;
                for (; j < unique; ++j) if (groups[i].monster_ == monsters[j]) break;
                if (j == unique) monsters[unique++] = groups[i].monster_;
            }
            if (total == 1) { self->resultKind_ = 0; self->monster_ = groups[0].monster_; }
            else if (unique > 1) { self->resultKind_ = 2; self->monster_ = 0; }
            else { self->resultKind_ = 1; self->monster_ = groups[0].monster_; }
        }
        func_ov023_021f454c(self);
        func_ov017_021cd218(self->info_->id_, self->totalGold_, self->turns_, self->members_, self->resultKind_, self->monster_);
        if (!self->info_->deferred_) {
            func_ov017_021cd590(self->info_->id_, self->experience_, 0, 0);
            func_ov017_021cd35c(self->info_->id_, self->drops_, self->dropCount_, self->dropMembers_);
            if (self->map_) EnqueueEventTag105_021cd4c8(self->info_->id_, self->mapItem_, self->dropMembers_);
            if (self->work_->bonusExperience_) {
                float multiplier = self->work_->experienceMultiplier_;
                unsigned int bonus = (unsigned int)((float)self->work_->bonusExperience_ * multiplier);
                GameState* actors = GameState::GetInstance();
                unsigned int rewards[4];
                for (int i = 0; i < 4; ++i)
                    if (TestBitAt0x34((unsigned char*)self->info_, (unsigned char)i)) rewards[i] = func_ov023_021f4098(self, i, bonus);
                func_ov017_021cd590(self->info_->id_, rewards, 0, 1);
                int index = GetField0x3acValue(actors);
                BattleBlock block;
                LoadBattleBlock020ac4c0(&block);
                AddSaturateField0x48(&block.battle_, rewards[index]);
                AddClamped24BitFieldAt0x24(&block.total_, rewards[index]);
                CopyInBattleField0x7540(&block);
            }
        }
    } else {
        if (resources->unknown_flag_42e2) SetCombatWorkFlags0x55f4(self, 0x2000000);
        if (func_0202c540(search) && self->info_->deferred_) messages->flag19b2_ = 0;
        if (!GetCombatWorkFlags0x55f4(self, 0x2000)) return self->endState_;
        if (!GetCombatWorkFlags0x55f4(self, 0x4000)) return self->endState_;
        if (!GetCombatWorkFlags0x55f4(self, 0x8000)) return self->endState_;
    }
    int groupCount = self->work_->groupCount_;
    BattleGroup* groups = self->work_->groups_;
    for (int i = 0; i < groupCount; ++i) {
        BattleGroup* group = &groups[i];
        Flags_021ed7e0 flags;
        ClearFlagBits10to24_021ed7e0(&flags);
        short record = group->record_;
        if (CopyIndexedRegion0x75f0(0, &record, (int*)&flags, 1)) {
            unsigned int count = flags.count_ + group->count_;
            if (count > 999) count = 999;
            flags.count_ = count;
            if (group->flag_) flags.flag_ = group->flag_;
            func_020abe84(0, &record, &flags, 1);
        }
    }
    MemberResult* member = _ZZ17GetGlobal021ffefcvE1s->members_;
    for (int i = 0; i < 4; ++i) {
        if (TestBitAt0x34((unsigned char*)self->info_, (unsigned char)i)) {
            ResultActor* actor = (ResultActor*)GetCombatantWithFlag0x100(game, i);
            if (actor) {
                member->member_ = i;
                member->vocation_ = actor->stats_->vocation_;
                member->level_ = GetTableValue(actor);
                member->blocked_ = IsFlag10088Set((S_10088*)actor);
                member->experience_ = self->experience_[i] + GetTableEntry138_021eea98((Obj021eea98*)actor);
                CharacterStats* stats = GetFieldAt0x150((unsigned char*)actor);
                member->stats_.field0_ = stats->field0_;
                member->stats_.field18_ = stats->field18_;
                member->stats_.field3c_ = stats->field3c_;
                member->stats_.field94_ = stats->field94_;
                member->stats_.experience_ = stats->experience_;
                member->stats_.levels_ = stats->levels_;
                member->stats_.revocations_ = stats->revocations_;
                member->stats_.field194_ = stats->field194_;
                member->stats_.field2f4_ = stats->field2f4_;
                member->stats_.field454_ = stats->field454_;
                member->stats_.field464_ = stats->field464_;
                member->stats_.field480_ = stats->field480_;
                member->stats_.field488_ = stats->field488_;
                member->stats_.field4a4_ = stats->field4a4_;
                member->stats_.points_ = stats->points_;
                member->stats_.field566_ = stats->field566_;
                member->stats_.field568_ = stats->field568_;
                member->stats_.field56a_ = stats->field56a_;
                member->stats_.field56b_ = stats->field56b_;
                member->stats_.field56c_ = stats->field56c_;
                member->stats_.field56d_ = stats->field56d_;
                member->stats_.field56e_ = stats->field56e_;
                member->stats_.field56f_ = stats->field56f_;
                member->stats_.field570_ = stats->field570_;
                member->stats_.field6f0_ = stats->field6f0_;
                member->stats_.field850_ = stats->field850_;
                member->stats_.field8ec_ = stats->field8ec_;
                member->stats_.field910_ = stats->field910_;
                member->stats_.field91c_ = stats->field91c_;
                member->stats_.field934_ = stats->field934_;
                member->stats_.field94c_ = stats->field94c_;
                member->stats_.vocation_ = stats->vocation_;
                member->stats_.field954_ = stats->field954_;
                member->stats_.field956_ = stats->field956_;
                member->stats_.field958_ = stats->field958_;
                member->stats_.field95a_ = stats->field95a_;
                member->stats_.field95c_ = stats->field95c_;
                member->stats_.field95e_ = stats->field95e_;
                member->stats_.field960_ = stats->field960_;
                member->stats_.field962_ = stats->field962_;
                strcpy(member->name_, actor->name_);
                ++member;
                ++_ZZ17GetGlobal021ffefcvE1s->memberCount_;
            }
        }
    }
    _ZZ17GetGlobal021ffefcvE1s->sequenceLength_ = CopyOutRegion0x571d((char*)game, _ZZ17GetGlobal021ffefcvE1s->sequence_);
    if (!self->info_->deferred_) _ZZ17GetGlobal021ffefcvE1s->nothing_ = func_ov023_021f5150(self, self->info_, self->experience_, &self->gold_, 0) ? 1 : 0;
    if (func_0202c540(search) && self->info_->kind_ == 0x13) SetEntryBitUnlessMatching020e39d4(GetData02153660(), 0, 1, 0x71e8);
    return 2;
}
