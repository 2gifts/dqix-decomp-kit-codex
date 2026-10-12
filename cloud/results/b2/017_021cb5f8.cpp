#include <globaldefs.h>
#include "GameState/GameState.h"

struct TacticsEntry021cb5f8 {
    unsigned char padding_0[2];
    unsigned char ids[8];
    unsigned char count : 4;
    unsigned char reserved : 4;
    unsigned char padding_b[0x18 - 0xb];
};
struct TacticsContainer021cb5f8 {
    unsigned char field_0;
    unsigned char reserved_1 : 4;
    unsigned char count : 2;
    unsigned char reserved_2 : 2;
    unsigned char padding_2[2];
    TacticsEntry021cb5f8 entries[3];
};
struct TacticsElement021cb5f8 {
    unsigned char padding_0[0x70];
    unsigned char primary[15];
    unsigned char padding_7f[0x93 - 0x7f];
    unsigned char secondary[15];
    unsigned char padding_a2[0xa4 - 0xa2];
};
struct TacticsState021cb5f8 {
    unsigned char padding_0[8];
    unsigned short id;
    unsigned char padding_a[0x158 - 0xa];
    TacticsElement021cb5f8 elements[8];
};
struct TacticsGlobals021cb5f8 {
    unsigned char padding_0[0x3718];
    void* node;
};
struct TacticsLow021cb5f8 {
    unsigned int s0 : 3, s1 : 3, s2 : 3, s3 : 3, s4 : 3;
    unsigned int s5 : 3, s6 : 3, s7 : 3, s8 : 3, s9 : 3;
};
struct TacticsHigh021cb5f8 {
    unsigned int s0 : 3, s1 : 3, s2 : 3, s3 : 3, s4 : 4;
};
struct TacticsFields021cb5f8 {
    unsigned short id;
    unsigned short slot;
    TacticsLow021cb5f8 low;
    TacticsHigh021cb5f8 high;
    unsigned char mode;
};
struct TacticsMessage021cb5f8 {
    unsigned char tag;
    unsigned char padding_1[3];
    TacticsFields021cb5f8 fields;
};
extern "C" TacticsState021cb5f8* func_ov017_021b8478(void*);
void* GetData02100044();
extern "C" void func_0205e330(void*, void*, int);

// USA: func_ov017_021cb5f8
extern "C" ARM void func_ov017_021cb5f8(TacticsContainer021cb5f8* self) {
    GameState::GetInstance();
    TacticsGlobals021cb5f8* globals = (TacticsGlobals021cb5f8*)func_ov017_0218b5b0();
    TacticsState021cb5f8* state = func_ov017_021b8478(globals->node);
    void* data = GetData02100044();
    for (int i = 0; i < self->count; i++) {
        TacticsEntry021cb5f8* entry = &self->entries[i];
        TacticsMessage021cb5f8 first;
        TacticsMessage021cb5f8 second;
        for (int j = 0; j < entry->count; j++) {
            unsigned char id = entry->ids[j];
            if (id >= 8) continue;
            TacticsElement021cb5f8* element = &state->elements[id];
            TacticsFields021cb5f8* fields = &first.fields;
            first.tag = 0x61;
            fields->id = state->id;
            fields->slot = entry->ids[j] + 0xc0;
            fields->mode = 1;
            fields->low.s0 = element->primary[0];
            fields->low.s1 = element->primary[1];
            fields->low.s2 = element->primary[2];
            fields->low.s3 = element->primary[3];
            fields->low.s4 = element->primary[4];
            fields->low.s5 = element->primary[5];
            fields->low.s6 = element->primary[6];
            fields->low.s7 = element->primary[7];
            fields->low.s8 = element->primary[8];
            fields->low.s9 = element->primary[9];
            fields->high.s0 = element->primary[10];
            fields->high.s1 = element->primary[11];
            fields->high.s2 = element->primary[12];
            fields->high.s3 = element->primary[13];
            fields->high.s4 = element->primary[14];
            func_0205e330(data, &first, 0);
            fields = &second.fields;
            second.tag = 0x61;
            fields->id = state->id;
            fields->slot = entry->ids[j] + 0xc0;
            fields->mode = 0;
            fields->low.s0 = element->secondary[0];
            fields->low.s1 = element->secondary[1];
            fields->low.s2 = element->secondary[2];
            fields->low.s3 = element->secondary[3];
            fields->low.s4 = element->secondary[4];
            fields->low.s5 = element->secondary[5];
            fields->low.s6 = element->secondary[6];
            fields->low.s7 = element->secondary[7];
            fields->low.s8 = element->secondary[8];
            fields->low.s9 = element->secondary[9];
            fields->high.s0 = element->secondary[10];
            fields->high.s1 = element->secondary[11];
            fields->high.s2 = element->secondary[12];
            fields->high.s3 = element->secondary[13];
            fields->high.s4 = element->secondary[14];
            func_0205e330(data, &second, 0);
        }
    }
}
