#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/Brightness.h"
#include "Graphics/Vector.h"
unsigned char* GetField0x3b0Value(GameState*);
void SetField0x7cClearFields0x1ec0x1ee(unsigned char*, unsigned short);
struct AngleTrig0202e9a4;
void SetAngleAndTrigTable0202e9a4(AngleTrig0202e9a4*, int);
extern "C" GameResources* func_ov017_0218b5b0();
struct Transition0204700c {
    unsigned int ticks; unsigned int field4; float yaw; float pitch;
    unsigned char pad0[0x1c-0x10]; unsigned char fade;
    unsigned char pad1[0x3c-0x1d]; Object3D object;
    Object3D* other;
};
// USA: func_0204700c
extern "C" ARM int func_0204700c(Transition0204700c* transition) {
    if (transition->ticks > 35) return 1;
    GameState* state = GameState::GetInstance();
    unsigned int ticks = state->GetTickCount();
    transition->yaw -= 8.0f;
    if (transition->yaw <= -180.0f) transition->yaw = 180.0f;
    float angle = 3.14f * (transition->yaw / 180.0f);
    transition->pitch -= 0.4333f;
    unsigned char* camera = GetField0x3b0Value(state);
    SetField0x7cClearFields0x1ec0x1ee(camera, fix32ReduceAngle0To2Pi((int)(4096.0f * angle)));
    SetAngleAndTrigTable0202e9a4((AngleTrig0202e9a4*)camera, (int)(4096.0f * transition->pitch));
    if (transition->other) transition->other->AdvanceEffects();
    else transition->object.AdvanceEffects();
    transition->ticks += ticks;
    if (transition->ticks > 15 && transition->fade) {
        GameResources* resources = func_ov017_0218b5b0();
        SetMainBrightness(resources, -16, 20);
        SetSubBrightness(resources, -16, 20);
        transition->fade = 0;
    }
    return transition->ticks > 35;
}
