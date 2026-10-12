#include <globaldefs.h>
#include "Util/Random.h"

struct LeaderSub0209afd4 {
    unsigned int key : 12;
    unsigned int countMin : 3;
    unsigned int countMax : 3;
    unsigned int weight1 : 3;
    unsigned int weight2 : 3;
    unsigned int weight3 : 3;
    unsigned int rest : 5;
    int unk4;
};

struct Member0209afd4 {
    unsigned int id : 12;
    unsigned int weight : 3;
    unsigned int countMin : 3;
    unsigned int countMax : 3;
    unsigned int rest : 11;
};

struct FindRowByIdRow {
    unsigned short id;
    unsigned short pad2;
    struct LeaderSub0209afd4 subs[6];
    struct Member0209afd4 members[6];
};

struct Entry0209b8d4;

struct Entry0209bd94 {
    unsigned short key;
    unsigned short pad2;
    unsigned int flagsLow : 10;
    unsigned int maxCount : 3;
    unsigned int flagsHigh : 19;
    unsigned char pad8[0x18];
};

struct EntryTable0209bd94 {
    struct Entry0209bd94 entries[6];
    int count;
};

struct EncounterState0209afd4 {
    unsigned char pad[0x60];
    struct EntryTable0209bd94 table;
};

struct EncounterGroup0209afd4 {
    short ids[3];
    short counts[3];
};

struct Shuffle0209afd4 {
    int slots[6][3];
};

struct LeaderSub0209afd4* FindSubByKeyAndId0209b8d4(struct Entry0209b8d4* table, int key, int id, int* out);
struct FindRowByIdRow* FindRowById(struct FindRowByIdRow* base, int key);
struct Entry0209bd94* FindEntryByHalfword0209bd94(struct EntryTable0209bd94* table, int key);

extern struct Shuffle0209afd4 data_020e8e2c;

// USA: func_0209afd4
extern "C" ARM void func_0209afd4(struct FindRowByIdRow* rows, int leaderId, int rowId, int leaderRowId, struct EncounterGroup0209afd4* out, struct EncounterState0209afd4* state) {
    struct Random* rnd;
    struct LeaderSub0209afd4* sub;
    struct FindRowByIdRow* row;
    struct Member0209afd4* member;
    struct Entry0209bd94* entry;
    int index;
    int kind;
    int roll;
    int total;
    int i;
    int k;
    int j;
    int l;
    int maxCount;
    int excess;
    int step;
    int prev;
    int slot;

    rnd = GetBTRandom();
    out->ids[0] = leaderId;
    index = 0;
    sub = FindSubByKeyAndId0209b8d4((struct Entry0209b8d4*)rows, leaderId, leaderRowId, &index);
    kind = 1;
    if ((unsigned short)(short)(leaderId - 0x26) <= 2) {
        out->counts[0] = 1;
    } else if (sub == NULL) {
        out->counts[0] = 1;
    } else {
        out->counts[0] = NextRandomMax(rnd, sub->countMax - sub->countMin + 1) + sub->countMin;
        roll = NextRandomMax(rnd, sub->weight1 + sub->weight2 + sub->weight3) + 1;
        if (roll <= sub->weight1) {
            kind = 1;
        } else if (roll <= sub->weight1 + sub->weight2) {
            kind = 2;
        } else if (roll <= sub->weight1 + sub->weight2 + sub->weight3) {
            kind = 3;
        }
        if (kind != 1) {
            row = FindRowById(rows, rowId);
            total = 0;
            for (i = 0; i < 6; i++) {
                member = &row->members[i];
                total += member->weight;
            }
            if (total == 0) {
                kind = 1;
            } else {
                roll = NextRandomMax(rnd, total) + 1;
                total = 0;
                for (j = 0; j < 6; j++) {
                    member = &row->members[j];
                    total += member->weight;
                    if (roll <= total) {
                        break;
                    }
                }
                out->ids[1] = member->id;
                out->counts[1] = NextRandomMax(rnd, member->countMax - member->countMin + 1) + member->countMin;
                if (kind != 2) {
                    total = 0;
                    for (k = 0; k < 6; k++) {
                        member = &row->members[k];
                        total += member->weight;
                    }
                    roll = NextRandomMax(rnd, total) + 1;
                    total = 0;
                    for (l = 0; l < 6; l++) {
                        member = &row->members[l];
                        total += member->weight;
                        if (roll <= total) {
                            break;
                        }
                    }
                    out->ids[2] = member->id;
                    out->counts[2] = NextRandomMax(rnd, member->countMax - member->countMin + 1) + member->countMin;
                }
            }
        }
    }

    entry = FindEntryByHalfword0209bd94(&state->table, rowId);
    if (entry != NULL) {
        maxCount = entry->maxCount;
    } else {
        maxCount = 5;
    }
    excess = out->counts[0] + out->counts[1] + out->counts[2] - maxCount;
    if (excess > 0) {
        step = excess / 3;
        if (step == 0) {
            step = 1;
        }
        slot = 0;
        while (excess > 0) {
            prev = out->counts[slot];
            out->counts[slot] -= step;
            if (out->counts[slot] < 0) {
                out->counts[slot] = 0;
            }
            if (slot == 0 && out->counts[slot] == 0) {
                out->counts[slot] = 1;
            }
            excess -= prev - out->counts[slot];
            slot++;
            if (slot >= 3) {
                slot = 0;
            }
        }
    }

    if (kind == 2) {
        if (NextRandomMax(rnd, 2) != 0) {
            short id = out->ids[0];
            short count = out->counts[0];
            out->ids[0] = out->ids[1];
            out->counts[0] = out->counts[1];
            out->ids[1] = id;
            out->counts[1] = count;
        }
    } else if (kind == 3) {
        struct Shuffle0209afd4 shuffle = data_020e8e2c;
        struct EncounterGroup0209afd4 saved;
        int pick = NextRandomMax(rnd, 6);
        saved = *out;
        out->ids[shuffle.slots[pick][0]] = saved.ids[0];
        out->counts[shuffle.slots[pick][0]] = saved.counts[0];
        out->ids[shuffle.slots[pick][1]] = saved.ids[1];
        out->counts[shuffle.slots[pick][1]] = saved.counts[1];
        out->ids[shuffle.slots[pick][2]] = saved.ids[2];
        out->counts[shuffle.slots[pick][2]] = saved.counts[2];
    }
}
