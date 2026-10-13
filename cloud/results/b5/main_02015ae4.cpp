#include <globaldefs.h>
#include <System/Matrix.h>

struct Obj0205eaa0;
void DispatchWithShortB4_0205eaa0(Obj0205eaa0*, int, int);
int fix32sin(int);
extern int data_02108760;

struct Obj02015ae4 {
    char pad0[0xe];
    unsigned short angle;
    unsigned short counter;
};

// USA: func_02015ae4
extern "C" ARM int func_02015ae4(Obj02015ae4* self) {
    unsigned short counter = self->counter;
    int firstDuration = 0xbff4;
    if ((float)counter <= (float)firstDuration / 4096.0f) {
        if (counter == 0) DispatchWithShortB4_0205eaa0((Obj0205eaa0*)&data_02108760, 13, 0);
        else if (counter == 16) DispatchWithShortB4_0205eaa0((Obj0205eaa0*)&data_02108760, 14, 0);
        unsigned int currentCounter = self->counter;
        int square = (self->counter * currentCounter) << 12;
        int coefficient = fix32_Divide(-7290, 0x8fee0);
        self->angle = FIX32_MULTIPLY(coefficient, square) + 0x647a;
        self->counter = self->counter + 1;
    } else {
        unsigned int elapsedCounter = self->counter;
        int firstDuration = 0xbff4;
        int elapsed = (int)(4096.0f * ((float)self->counter - (float)firstDuration / 4096.0f));
        int totalDuration = 0x17fe8;
        if ((float)elapsedCounter <= (float)totalDuration / 4096.0f) {
            int remaining = 0x1000 - fix32_Divide(elapsed, 0x17fe8);
            int coefficient = fix32_Divide(0xc90f, 0x17fe8);
            int angle = FIX32_MULTIPLY(coefficient, elapsed);
            if (angle > 0x647a) angle = 0x647a;
            int correction = FIX32_MULTIPLY(fix32sin(angle), -0x101);
            self->angle = FIX32_MULTIPLY(correction, remaining) + 0x4800;
            self->counter = self->counter + 1;
        } else {
            self->angle = 0x4800;
            return 1;
        }
    }
    return 0;
}
