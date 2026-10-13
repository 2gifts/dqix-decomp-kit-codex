#include <globaldefs.h>
#include "GameState/GameState.h"

struct CameraFraming {
    unsigned char pad_00[0x10];
    Vector3i target;
    unsigned char pad_1c[0x204];
    unsigned char state;
    unsigned char pad_221[0x17];
    int transition;
    int delay;
};
extern const Vector3i data_ov000_02183280;
extern "C" void func_ov000_0216d370(CameraFraming*, int, int, int);
void SetFlagOrState0216d530(void*, int);
void ApplyVec3Tail(void*, int*);

// USA: func_ov000_0216dbf0
extern "C" ARM void func_ov000_0216dbf0(CameraFraming* camera, int* ids, int count, int mode) {
    func_ov000_0216d370(camera, 1, 1, 1);
    camera->state = 3;
    Vector3i target = data_ov000_02183280;
    int framing[3];
    framing[0] = 0x5e38;
    GameState* game = GameState::GetInstance();
    int distance = 0;
    int radius = 0;
    int height = 0x2000;
    if (count == 1) {
        GameObject* combatant = game->GetGameObjectByIndex(*ids);
        if (combatant) {
            SetFlagOrState0216d530(camera, *ids);
            radius = combatant->obj3D_.GetRadius() >> 1;
            height = combatant->obj3D_.GetHeight();
        }
    } else {
        int i = 0;
        while (i < count) {
            GameObject* combatant = game->GetGameObjectByIndex(*ids);
            if (combatant) {
                Vector3fix position = combatant->obj3D_.position_;
                int length = Vector3fix_Length(&position);
                if (distance < length) {
                    distance = length;
                    radius = combatant->obj3D_.GetRadius() >> 1;
                }
                if (height < combatant->obj3D_.GetHeight()) height = combatant->obj3D_.GetHeight();
            }
            i++;
            ids++;
        }
    }
    int extent = FIX32_MULTIPLY(0x3000, distance + radius);
    int tall = FIX32_MULTIPLY(0x3000, height);
    framing[1] = 0x1000;
    target.y = height >> 1;
    if (tall < extent) framing[2] = extent;
    else framing[2] = tall;
    if (mode == 0) {
        framing[1] = 0x1000;
        if (framing[2] < 0x7000) framing[2] = 0x7000;
        else if (framing[2] > 0x10000) framing[1] += (framing[2] - 0x10000) / 2;
    } else {
        target.y = 0xb85;
        framing[1] = 0x3999;
        if (framing[2] < 0xe000) framing[2] = 0xe000;
    }
    camera->target = target;
    ApplyVec3Tail(camera, framing);
    camera->transition = 0x1c;
    camera->delay = 0x14;
}
