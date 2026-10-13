#include <globaldefs.h>
#include <std_library_functions.h>

struct Unit { char pad0[0x25]; unsigned char suffix_; char pad26[0xa4 - 0x26]; };
struct Container02070e60 { char storage_[4]; };
struct UnitTemplate { char storage_[0x58]; };
struct Element02070e60 { char pad0[0x2c]; UnitTemplate unit_; };
struct UnitTable { char pad0[0x400]; Container02070e60 container_; };
struct BattleData {
    char pad0[0x158];
    Unit units_[1];
    char pad1FC[0x284 - 0x1fc];
    UnitTable table_;
};
struct CombatGroup {
    unsigned short key_;
    unsigned char members_[8];
    unsigned char memberCount_ : 4;
    unsigned char matched_ : 4;
    char padB[0x18 - 0xb];
};
struct SpeciesGroup {
    unsigned char nextSuffix_;
    unsigned char memberCount_;
    unsigned short key_;
    unsigned int suffixMask_;
};
struct Battle {
    char pad0[0x81b1];
    unsigned char counter_ : 4;
    unsigned char groupCount_ : 2;
    unsigned char speciesCount_ : 2;
    char pad81B2[2];
    CombatGroup groups_[3];
    SpeciesGroup species_[4];
    char pad821C[0x8e18 - 0x821c];
    BattleData* data_;
};
Element02070e60* SearchWithLow15Comparator02070fd0(Container02070e60*, int);
int IsField0xcNonNegative(int*);
extern "C" void func_02089630(Unit*, UnitTemplate*, int);

// USA: func_ov000_0215ed8c
extern "C" ARM void func_ov000_0215ed8c(Battle* self) {
    Container02070e60* table = &self->data_->table_.container_;
    CombatGroup* groups = self->groups_;
    SpeciesGroup* species = self->species_;
    unsigned char unitIndex = 0;
    int totals[4] = {0};
    int i;
    int key;
    unsigned char* members;
    Unit* unit;
    Element02070e60* entry;
    CombatGroup* group;
    for (i = 0; i < self->groupCount_; ++i) {
        group = &groups[i];
        key = group->key_;
        int speciesIndex;
        for (speciesIndex = 0; speciesIndex < self->speciesCount_; ++speciesIndex) {
            if (key == species[speciesIndex].key_) break;
        }
        members = groups[i].members_;
        memset(members, 0, 8);
        totals[speciesIndex] += groups[i].memberCount_;
        for (int j = 0; j < group->memberCount_; ++j) {
            members[j] = unitIndex;
            unit = &self->data_->units_[unitIndex];
            entry = SearchWithLow15Comparator02070fd0(table, key);
            if (entry) {
                func_02089630(unit, &entry->unit_, IsField0xcNonNegative((int*)self->data_) != 0);
                for (;;) {
                    if (!(species[speciesIndex].suffixMask_ & (1 << species[speciesIndex].nextSuffix_))) break;
                    ++species[speciesIndex].nextSuffix_;
                    if (species[speciesIndex].nextSuffix_ >= 18) species[speciesIndex].nextSuffix_ = 1;
                }
                unit->suffix_ = species[speciesIndex].nextSuffix_;
                species[speciesIndex].suffixMask_ |= 1 << unit->suffix_;
                ++species[speciesIndex].nextSuffix_;
                if (species[speciesIndex].nextSuffix_ >= 18) species[speciesIndex].nextSuffix_ = 1;
            }
            ++unitIndex;
        }
    }
    for (int j = 0; j < 4; ++j) species[j].memberCount_ = totals[j];
    for (int j = 0; j < self->speciesCount_; ++j) {
        if (species[j].memberCount_ <= 1) {
            int key = species[j].key_;
            int groupIndex;
            for (groupIndex = 0; groupIndex < self->groupCount_; ++groupIndex) {
                if (key == groups[groupIndex].key_) break;
            }
            self->data_->units_[groups[groupIndex].members_[0]].suffix_ = 0;
        }
    }
}
