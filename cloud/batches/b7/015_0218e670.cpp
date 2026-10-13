#include <globaldefs.h>
#include "World/Object3D.h"
#include "Graphics/NSBXX/RenderConfig.h"
#include "GameState/GameState.h"

struct BattleCamera { int field0; Vector3i target; Vector3i position; };
BattleCamera* GetFieldIfFlag2(char*);
int TestFlag0SetAndFlag1Clear(unsigned short*, int);
extern unsigned short data_02114e30;
struct BoneCapture { Matrix4x3* second; Matrix4x3* third; Matrix4x3* first; };
extern BoneCapture data_ov015_02194564;
extern "C" void func_020c5414();
extern "C" void func_0202eab8(BattleCamera*);
struct BattleModel {
    char pad0[0x24];
    Object3D* object;
    char pad28[0x12];
    unsigned char paused;
};

// USA: func_ov015_0218e670
extern "C" ARM void func_ov015_0218e670(BattleModel* self) {
    GameState* game = GameState::GetInstance();
    if (!game) return;
    BattleCamera* camera = GetFieldIfFlag2((char*)game);
    if (!camera) return;
    int update = 0;
    if (self->paused) {
        if (TestFlag0SetAndFlag1Clear(&data_02114e30, 0x100)) update = 1;
    } else update = 1;
    if (!update) return;
    Matrix4x3 first, second, third;
    data_ov015_02194564.first = &first;
    data_ov015_02194564.second = &second;
    data_ov015_02194564.third = &third;
    func_020c5414();
    RenderConfig::SubmitToFifo();
    self->object->MaybeUpdateBonePositions();
    data_ov015_02194564.first = 0;
    data_ov015_02194564.second = 0;
    data_ov015_02194564.third = 0;
    Vector3i a = {first.translation.x, first.translation.y, first.translation.z};
    Vector3i b = {second.translation.x, second.translation.y, second.translation.z};
    Vector3i c = {third.translation.x, third.translation.y, third.translation.z};
    Vector3i scale = self->object->GetScale();
    Vector3fixMultiply(&a, &scale, &a);
    Vector3fixMultiply(&b, &scale, &b);
    Vector3fixMultiply(&c, &scale, &c);
    Vector3i position = self->object->position_;
    Vector3fix_Add(&a, &position, &a);
    Vector3fix_Add(&b, &position, &b);
    Vector3fix_Add(&c, &position, &c);
    camera->position = b;
    camera->target = a;
    func_0202eab8(camera);
}
