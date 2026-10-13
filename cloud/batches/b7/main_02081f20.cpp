#include <globaldefs.h>

struct State02081f20 {
    unsigned short id;
    unsigned char status;
};

struct Obj0208203c {
    State02081f20* state;
    short unused4;
    signed char startValue;
    signed char reloadValue;
    signed char timer;
};

struct IdTable02081f20 {
    unsigned short ids[4];
};

extern IdTable02081f20 data_020e8a84;
extern unsigned short data_02114e30;

int TestFlag0SetAndFlag1Clear(unsigned short* flags, int id);
int TestFlagMask(unsigned short* flags, int id);
void ResetWithSub0208203c(Obj0208203c* obj);

// USA: func_02081f20
extern "C" ARM int func_02081f20(Obj0208203c* obj, int amount) {
    if (obj->state == 0) {
        return 0;
    }
    obj->state->status = 0;

    unsigned short id = obj->state->id;
    if (id == 0) {
        IdTable02081f20 table = data_020e8a84;
        int i;
        for (i = 0; i < 4; i++) {
            unsigned short candidate = table.ids[i];
            if (TestFlag0SetAndFlag1Clear(&data_02114e30, candidate)) {
                obj->state->id = candidate;
                obj->timer = obj->startValue;
                obj->state->status = 1;
                break;
            }
        }
    } else if (id != 0) {
        if (TestFlagMask(&data_02114e30, id)) {
            obj->timer = obj->timer - (char)amount;
            int remaining = obj->timer;
            if (remaining < 0) {
                obj->timer = remaining + obj->reloadValue;
                obj->state->status = 2;
            }
        } else {
            ResetWithSub0208203c(obj);
        }
    }
    return obj->state->status;
}
