#include <globaldefs.h>

struct Random;
struct EntryTable0209bd94;

struct Elem8_0209b944 {
    unsigned int pad : 18;
    unsigned int low : 3;
    unsigned int mid : 3;
    unsigned int high : 3;
    unsigned int rest : 5;
    int field4;
};

struct PickEntry {
    unsigned int id : 12;
    unsigned int weight : 3;
    unsigned int minVal : 3;
    unsigned int maxVal : 3;
    unsigned int rest : 11;
};

struct SlotEntry0209b944 {
    unsigned short id;
    char pad[0x32];
    PickEntry pick[6];
};

struct TableEntry {
    int field0;
    unsigned int pad : 10;
    unsigned int cost : 3;
    unsigned int rest : 19;
};

struct Trio {
    short v[3];
};

struct PickResult {
    short id[3];
    short val[3];
};

struct PickContext {
    char pad[0x60];
    char table[1];
};

Random* GetBTRandom();
Elem8_0209b944* FindSlotById0209b944(SlotEntry0209b944*, int, int, int*);
extern "C" int rand();
TableEntry* FindEntryByHalfword0209bd94(EntryTable0209bd94*, int);
int NextRandomMax(Random*, int);

extern int data_020e8e74[][3];
extern int data_020e8e78[][3];
extern int data_020e8e7c[][3];

// USA: func_0209b448
ARM void PickSlotRewards0209b448(SlotEntry0209b944* entries, int key, PickResult* out, PickContext* ctx) {
    Random* rng = GetBTRandom();
    Trio ids;
    Trio vals;
    int idx = 0;
    Elem8_0209b944* elem = FindSlotById0209b944(entries, 0, key, &idx);
    PickEntry* p;
    int total;
    int i;
    int tier = 1;
    if (elem == 0) {
        out->val[0] = 1;
        out->id[0] = 0x10;
    } else {
        int sum = elem->low + elem->mid + elem->high;
        unsigned int roll = rand() % sum + 1;
        if (roll > elem->low) {
            if (roll <= elem->low + elem->mid) {
                tier = 2;
            } else if (roll <= elem->low + elem->mid + elem->high) {
                tier = 3;
            }
        }
        total = 0;
        for (i = 0; i < 6; i++) {
            p = &entries[idx].pick[i];
            total += p->weight;
        }
        if (total == 0) {
            tier = 1;
            out->val[0] = 1;
            out->id[0] = 0x10;
        } else {
            {
                int r = rand() % total + 1;
                int i = 0;
                int acc = 0;
                for (; i < 6; i++) {
                    p = &entries[idx].pick[i];
                    acc += p->weight;
                    if (r <= acc) {
                        break;
                    }
                }
                out->id[0] = p->id;
                int range = p->maxVal - p->minVal;
                out->val[0] = rand() % (range + 1) + p->minVal;
            }
            if (tier != 1) {
            {
                int r = rand() % total + 1;
                int i = 0;
                int acc = 0;
                for (; i < 6; i++) {
                    p = &entries[idx].pick[i];
                    acc += p->weight;
                    if (r <= acc) {
                        break;
                    }
                }
                out->id[1] = p->id;
                int range = p->maxVal - p->minVal;
                out->val[1] = rand() % (range + 1) + p->minVal;
            }
            if (tier != 2) {
            {
                int r = rand() % total + 1;
                int i = 0;
                int acc = 0;
                for (; i < 6; i++) {
                    p = &entries[idx].pick[i];
                    acc += p->weight;
                    if (r <= acc) {
                        break;
                    }
                }
                out->id[2] = p->id;
                int range = p->maxVal - p->minVal;
                out->val[2] = rand() % (range + 1) + p->minVal;
            }
            }
            }
        }
    }
    TableEntry* te = FindEntryByHalfword0209bd94((EntryTable0209bd94*)ctx->table, key);
    int cost = te ? te->cost : 5;
    int excess = out->val[0] + out->val[1] + out->val[2] - cost;
    if (excess > 0) {
        int step = excess / 3;
        if (step == 0) {
            step = 1;
        }
        int j = 0;
        while (excess > 0) {
            int before = out->val[j];
            out->val[j] -= step;
            if (out->val[j] < 0) {
                out->val[j] = 0;
            }
            if (j == 0 && out->val[j] == 0) {
                out->val[j] = 1;
            }
            excess -= before - out->val[j];
            if (++j >= 3) {
                j = 0;
            }
        }
    }
    if (tier == 2) {
        if (NextRandomMax(rng, 2) != 0) {
            short a = out->id[0];
            short b = out->val[0];
            out->id[0] = out->id[1];
            out->val[0] = out->val[1];
            out->id[1] = a;
            out->val[1] = b;
        }
    } else if (tier == 3) {
        int n = NextRandomMax(rng, 6);
        ids = *(Trio*)&out->id;
        vals = *(Trio*)&out->val;
        int k0 = data_020e8e74[n][0];
        out->id[k0] = ids.v[0];
        out->val[k0] = vals.v[0];
        int k1 = data_020e8e78[n][0];
        out->id[k1] = ids.v[1];
        out->val[k1] = vals.v[1];
        int k2 = data_020e8e7c[n][0];
        out->id[k2] = ids.v[2];
        out->val[k2] = vals.v[2];
    }
}
