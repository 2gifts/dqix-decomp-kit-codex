#include <globaldefs.h>
#include "GameState/GameState.h"
#include "System/Graphics.h"

struct Vec3copy0202ec84 { unsigned int v[3]; };

struct Vec3Target0203a46c {
    char unk[0x1c];
    int x;
    int y;
    int z;
};

int GetField0x3b0Value(GameState*);
void ComputeTwoFromVec3_0202ec84(void*, struct Vec3copy0202ec84*, int*, int*);
void SetVec3At0x1c(struct Vec3Target0203a46c*, int, int, int);
void StoreVec3At0x34(unsigned int*, unsigned int, unsigned int, unsigned int);
extern "C" void func_02047554(void* sprite, int digit, int flag);

extern const int data_020e7830[5];

struct Sprite0203a0b4 {
    struct Vec3Target0203a46c pos;
    char pad28[0x82 - 0x28];
    short alpha;
    char pad84[0x88 - 0x84];
};

struct Slot0203a0b4 {
    unsigned short value;
    short offsetY;
    signed char offsetX;
    signed char timer;
    unsigned char digits;
    unsigned char spriteIndex;
    short velocity;
    short position;
    struct Vec3copy0202ec84 anchor;
};

struct Obj0203a0b4 {
    struct Sprite0203a0b4 digitSprites[5];
    struct Sprite0203a0b4 shadowSprites[5];
    struct Slot0203a0b4 slots[16];
    int cursor;
    int limit;
};

// USA: func_0203a0b4
ARM void UpdateFloatingNumbers0203a0b4(struct Obj0203a0b4* obj) {
    int px;
    int n;
    int i;
    struct Slot0203a0b4* slot;
    int baseX;
    int digit;
    int left;
    void* camera = (void*)GetField0x3b0Value(GameState::GetInstance());
    if (camera == 0) return;
    i = obj->cursor;
    while (i != obj->limit) {
        slot = &obj->slots[i];
        n = slot->timer - 5;
        if (n <= 4) n = n * 7;
        else n = 31;
        if (slot->timer < 35 && n > 0) {
            int spriteIndex = slot->spriteIndex;
            int value = slot->value;
            digit = 1;
            int x, y;
            ComputeTwoFromVec3_0202ec84(camera, &slot->anchor, &x, &y);
            x += slot->offsetX;
            y = y + slot->offsetY - 20;
            if (x < 16) x = 16;
            if (x > 240) x = 240;
            baseX = x;
            if (y < 16) y = 16;
            if (y > 176) y = 176;
            int baseY = y;
            px = baseX + (slot->digits * 8 / 2 - 8);
            x = px;
            left = slot->digits - 1;
            while (value > 0) {
                bool show = true;
                int dx = 0, dy = 0, dz = 0;
                int scale = 0x1000;
                int k = 34 - slot->timer - left * 3;
                if (k < 0) {
                    k = 0;
                    show = false;
                } else if (k < 5) {
                    int t = data_020e7830[k];
                    scale = t + 0x1000;
                    dx = (int)(((long long)t * -0x4000 + 0x800) >> 12);
                    dy = (int)(((long long)t * -0x8000 + 0x800) >> 12);
                    dz = (int)(((long long)t * 0x5000 + 0x800) >> 12);
                }
                if (show) {
                    SetVec3At0x1c(&obj->digitSprites[spriteIndex].pos, dx + (px << 12), dy + ((y - 8) << 12), dz + (digit << 12));
                    obj->digitSprites[spriteIndex].alpha = n;
                    StoreVec3At0x34((unsigned int*)&obj->digitSprites[spriteIndex], scale, scale, 0x1000);
                    GXFIFO_MATRIX_PUSH = 0;
                    func_02047554(&obj->digitSprites[spriteIndex], value % 10, 1);
                    GXFIFO_MATRIX_POP = 1;
                }
                value = value / 10;
                x -= 8;
                px = x;
                digit++;
                left--;
            }
            if (n > 0) {
                float s = (float)obj->slots[i].position / 4096.0f;
                baseX -= (int)(16.0f * s);
                int y2 = baseY - (int)(8.0f * s) - 8;
                SetVec3At0x1c(&obj->shadowSprites[spriteIndex].pos, baseX << 12, y2 << 12, 0);
                StoreVec3At0x34((unsigned int*)&obj->shadowSprites[spriteIndex], obj->slots[i].position, obj->slots[i].position, 0x1000);
                obj->shadowSprites[spriteIndex].alpha = n;
                GXFIFO_MATRIX_PUSH = 0;
                func_02047554(&obj->shadowSprites[spriteIndex], 0, 1);
                GXFIFO_MATRIX_POP = 1;
            }
        }
        i++;
        if (i >= 16) i = 0;
    }
}
