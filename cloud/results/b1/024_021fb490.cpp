#include <globaldefs.h>
#include "std_library_functions.h"

struct Action_021fb490 {
    unsigned int field_0x0;
    unsigned int code : 12;
    unsigned int unused_0x4 : 20;
    unsigned int unused_0x8 : 8;
    unsigned int mode : 2;
    unsigned int unused_0xa : 4;
    unsigned int element : 8;
    unsigned int unused_0x16 : 10;
    unsigned int field_0xc;
    unsigned int flags;
    unsigned int unused_0x14 : 28;
    unsigned int targetKind : 4;
    unsigned int field_0x18;
    unsigned int unused_0x1c : 14;
    unsigned int range : 5;
    unsigned int unused_0x1f : 13;
};
struct Entry_021fb490 {
    float value;
    unsigned char id, flag, index, percent, kind;
    unsigned char hits : 6;
    unsigned char multi : 1;
    unsigned char unused : 1;
    unsigned char padding[2];
};
struct AddEntryList_021f6a1c {
    Entry_021fb490 entries[16];
    int count;
    float threshold;
    AddEntryList_021f6a1c() { memset(this, 0, sizeof(*this)); }
};
struct Stats_021fb490 {
    unsigned char padding[0x14];
    unsigned int flags;
};
struct Status_021fb490 {
    unsigned char padding[0xa];
    unsigned short code : 12;
    unsigned short sealed : 1;
};
struct TargetFlags_021fb490 {
    unsigned int defender : 1;
    unsigned int guarded : 1;
};
struct Field_021fb490 {
    unsigned char padding[0x2f4];
    TargetFlags_021fb490 targetFlags;
};
struct Combatant_021fb490 {
    unsigned char padding[4];
    short id;
    unsigned char padding_0x6[0x138 - 0x6];
    Stats_021fb490* stats;
    unsigned char padding_0x13c[0x144 - 0x13c];
    Status_021fb490* status;
    unsigned char padding_0x148[0x150 - 0x148];
    Field_021fb490* field;
    unsigned char padding_0x154[0x17c - 0x154];
    unsigned char side;
};
struct ActionWork_021fb490 {
    unsigned char padding[0x3ec];
    int scaling;
    Action_021fb490* action;
    unsigned char padding_0x3f4[8];
    AddEntryList_021f6a1c* list;
    Entry_021fb490 savedEntry;
    Combatant_021fb490* selected;
    int selectedIndex;
};
struct Context_021fb490 {
    void* battle;
    short id;
    unsigned short field_0x6;
    Combatant_021fb490* actor;
    unsigned char padding_0xc[0x2c - 0xc];
    unsigned char skipped;
    unsigned char padding_0x2d[0x34 - 0x2d];
    unsigned char isPrimary;
    unsigned char padding_0x35[0x7c - 0x35];
    Combatant_021fb490* targets[8];
    int count;
    unsigned char padding_0xa0[0x25c - 0xa0];
    ActionWork_021fb490 work;
};
struct FlagObj_021da998;
struct FlagObj_021dd260;
int GetFieldAt0x150(unsigned char*);
void* GetData02108e10();
void* LookupElementByKey02079ee0(void*, int);
float LookupTableD0Clamped(int);
void AddEntryIfUnderLimit16_021f6a1c(AddEntryList_021f6a1c*, void*);
int IsFlagBit8388608Set_021da998(FlagObj_021da998*);
int IsFlagBit24Set_021dd260(FlagObj_021dd260*);
extern "C" void func_ov024_021f8938(void*, float*, float*, int, Action_021fb490*, Combatant_021fb490*);
extern "C" void func_ov024_021fa7ec(Context_021fb490*, float*, float*, float*, int*, float, float, void*, Combatant_021fb490*, int);
extern "C" int func_ov024_021f8d80(Context_021fb490*, int*, Action_021fb490*, Combatant_021fb490**, int, float*, float*, int, Combatant_021fb490*);
extern "C" void func_ov024_021f6a4c(void*, void*);
extern "C" void func_ov024_021fd858(void*);
extern "C" void func_ov024_021f9874(void*, void*, int, int);

extern unsigned char data_ov024_021ff004[];
extern unsigned char data_ov024_021ff002[];
extern unsigned char data_ov024_021ff003[];

// USA: func_ov024_021fb490
extern "C" ARM void func_ov024_021fb490(Context_021fb490* context) {
    void* database;
    int scaling;
    unsigned char multi;
    Action_021fb490* action;
    void* element;
    Combatant_021fb490** targets;
    unsigned int range;
    int hits;
    int kind;
    int grouped;
    int j;
    Combatant_021fb490* actor;
    int columns;
    int count;
    int boosts;
    int i;
    int discard;
    Combatant_021fb490* target;
    int rows;
    actor = context->actor;
    GetFieldAt0x150((unsigned char*)actor);
    database = GetData02108e10();
    action = context->work.action;
    scaling = context->work.scaling;
    if (action->mode == 2) return;
    float value = 0;
    float spread = 0;
    int bestIndex = -1;
    func_ov024_021f8938(context, &value, &spread, scaling, action, actor);
    targets = context->targets;
    count = context->count;
    element = LookupElementByKey02079ee0(database, action->element);
    float hi[8];
    float lo[8];
    float avg[8];
    for (i = 0; i < count; i++) {
        lo[i] = 0;
        avg[i] = 0;
        hi[i] = 0;
        float* outHi = &hi[i];
        float* outLo = &lo[i];
        float* outAvg = &avg[i];
        target = targets[i];
        if (target->status && target->status->sealed && !(action->flags & 0x1000000)) continue;
        func_ov024_021fa7ec(context, outHi, outLo, outAvg, &bestIndex, value, spread, element, target, i);
    }
    kind = action->targetKind;
    if (kind == 5) {
        TargetFlags_021fb490* flags = &actor->field->targetFlags;
        if (flags) {
            if (flags->guarded) kind = 3;
            else if (flags->defender) kind = 4;
            else kind = 2;
        }
    }
    if (kind != 2 && kind != 4 && kind != 3) return;
    range = action->range;
    if (range > 11) return;
    int chosen = -1;
    rows = data_ov024_021ff002[range * 3];
    columns = data_ov024_021ff003[range * 3];
    hits = (short)rows * (short)columns;
    multi = data_ov024_021ff004[range * 3];
    if (kind == 2 && rows == 1) {
        func_ov024_021f8d80(context, &chosen, action, context->targets, context->count, avg, lo, hits, actor);
    }
    AddEntryList_021f6a1c list;
    memset(&list, 0, sizeof(list));
    grouped = 0;
    discard = 0;
    boosts = 0;
    for (j = 0; j < context->count; j++) {
        context->skipped = 0;
        if (chosen >= 0 && chosen != j) continue;
        if (bestIndex == j) context->isPrimary = 1;
        target = targets[j];
        grouped++;
        if ((action->flags & 0x400) && (target->stats->flags & 0x200)) discard = 1;
        if (kind != 2 && (context->work.action->flags & 0x20000)) {
            avg[j] *= LookupTableD0Clamped((signed char)boosts);
            boosts++;
        }
        Entry_021fb490 entry;
        entry.flag = 1;
        entry.percent = 100;
        entry.kind = 0;
        entry.value = avg[j];
        entry.index = j;
        entry.hits = hits;
        entry.multi = multi;
        AddEntryIfUnderLimit16_021f6a1c(&list, &entry);
        if (hi[j] >= 0.1f && !IsFlagBit8388608Set_021da998((FlagObj_021da998*)actor) &&
            !IsFlagBit24Set_021dd260((FlagObj_021dd260*)actor)) {
            context->work.list = &list;
            func_ov024_021f6a4c(&context->work.savedEntry, &entry);
            context->work.selected = target;
            context->work.selectedIndex = j;
            func_ov024_021fd858(context);
        }
        int flush = 0;
        if (j == context->count - 1) flush = 1;
        else if (kind == 2) flush = 1;
        else if (kind == 4) {
            Combatant_021fb490* next = targets[j + 1];
            if (next->side != target->side) flush = 1;
        }
        if (!flush) continue;
        if (grouped >= 2 && multi) discard = 1;
        if (grouped <= 1) multi = 0;
        if (!discard) {
            int side = target->side;
            int id = (unsigned char)target->id;
            if (kind == 4) {
                id = 0xff;
            } else if (kind == 3) {
                id = 0xff;
                side = 0xff;
            }
            func_ov024_021f9874(context, &list, side, id);
        }
        memset(&list, 0, sizeof(list));
        grouped = 0;
        discard = 0;
        boosts = 0;
    }
}
