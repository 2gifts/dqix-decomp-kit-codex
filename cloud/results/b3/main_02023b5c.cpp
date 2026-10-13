#include <globaldefs.h>
#include "System/Matrix.h"
#include "GameState/GameState.h"

extern "C" void func_02012fe4(void);
extern "C" void func_0202ae18(void);
extern "C" int func_0202c540(void);
extern "C" int _u32_div_f(int, int);
extern "C" int _s32_div_f(int, int);
extern "C" int abs(int);
int SetFlag0x8eIfBitTestFails(char*, int);
void SetElementFields0202756c(void*, int, int, int, unsigned char, unsigned char, unsigned short,
                              unsigned char, int, int);
extern "C" void ColorEffect_ConfigureAlphaBlend(void*, int, int, int, int);

struct ActionReceiver {
    unsigned char unknown0[0x38];
    fix32_t scale;
    unsigned char unknown3c[0x77c - 0x3c];
    int field77c;
    unsigned char count;
    unsigned char unknown781[0x794 - 0x781];
    unsigned char entries[0x9b4 - 0x794];
    unsigned int field9b4;
    unsigned char unknown9b8[0x9ba - 0x9b8];
    unsigned char field9ba;
};

// USA: func_02023b5c
extern "C" ARM void func_02023b5c(ActionReceiver* receiver, int tileX, int tileY, int xOffset,
                                  int yOffset, unsigned char flag) {
    func_02012fe4();
    if (receiver->field77c == 0) return;
    func_0202ae18();
    if (func_0202c540() != 0) return;

    GameState* game    = GameState::GetInstance();
    unsigned int sum   = receiver->field9b4 + (unsigned int)game->GetEffectiveDeltaTime();
    unsigned int rem   = sum % 0x300;
    receiver->field9b4 = rem;
    int amplitude      = abs(((int)rem - 0x180) / 0x18) + 2;
    unsigned char* entries = receiver->entries;
    if (amplitude > 0x10) amplitude = 0x10;
    unsigned char flagByte = flag;
    int blend              = 0x10 - amplitude;
    int yOff               = yOffset;
    unsigned char count    = receiver->count;

    for (int i = 0; i < count; i++) {
        if (!SetFlag0x8eIfBitTestFails((char*)receiver, i)) continue;
        Vector3i position;
        position      = *(Vector3i*)(entries + i * 0x1c);
        fix32_t scale = receiver->scale;
        int x         = (int)((float)FIX32_MULTIPLY(position.x, scale) / 4096.0f) - tileX;
        int y         = (int)((float)FIX32_MULTIPLY(position.z, scale) / 4096.0f) - tileY;
        if (flagByte) {
            y = -((int)((float)FIX32_MULTIPLY(position.y, scale) / 4096.0f) - tileY);
        }
        if (receiver->field9ba != 0) {
            ColorEffect_ConfigureAlphaBlend((void*)0x04001050, 0, 3, amplitude, blend);
            SetElementFields0202756c(receiver, (x + xOffset) << 12, (y + yOff) << 12, 0x17,
                                     (unsigned char)(i + 0x60), 1, 0xff, 0xff, 0x1000, 0x1000);
        }
    }
}
