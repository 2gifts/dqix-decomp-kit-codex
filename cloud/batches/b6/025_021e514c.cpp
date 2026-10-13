#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Vector.h"

struct List02160094;
struct List021600f8;
struct SourceNode { char pad0[0x20]; unsigned short actor_; };
struct TargetNode { char pad0[0xe]; short actor_; };
struct Obj02033874;
struct PositionCommand {
    char pad0[8]; unsigned char selection_, mode_, reference_, heightMode_;
    int maximumHeight_, minimumHeight_; Vector3fix offset_; unsigned char reverse_;
};
struct CameraView { int field0_; Vector3fix eye_, target_; };
SourceNode* GetNodeAtIndex02160094(List02160094*, int);
TargetNode* GetNodeAtIndex021600f8(List021600f8*, int);
int DispatchByIndex021820bc(void*, int, int, int);
CameraView* GetField0x3b0Value(GameState*);
int GetField0x70(void*);
void SetVecYFromValue02033874(Obj02033874*, int);
void StoreVec3AtField0x50(unsigned char*, int, int, int);
extern "C" void __clear(void*, int);

// USA: func_ov025_021e514c
extern "C" ARM int func_ov025_021e514c(PositionCommand* command, void* lists, int unused, void* context) {
    int actors[8];
    Vector3fix position;
    Matrix4x3 cameraRotation;
    Matrix4x3 actorRotation;
    Vector3fix relative;
    GameState* game = GameState::GetInstance();
    SourceNode* source = GetNodeAtIndex02160094((List02160094*)lists, 0);
    TargetNode* target = GetNodeAtIndex021600f8((List021600f8*)lists, 0);
    int count = DispatchByIndex021820bc(context, (int)lists, command->selection_, (int)actors);
    __clear(&position, sizeof(position));
    int angle = 0;
    int side = 0;
    unsigned char sourceParty = 0;
    unsigned char targetParty = 0;
    if (source) sourceParty = source->actor_ <= 3 ? 1 : 0;
    if (target) {
        int party = 0;
        if (target->actor_ >= 0 && target->actor_ <= 3) party = 1;
        targetParty = party;
    }
    switch (command->mode_) {
    case 1: side = sourceParty ? 1 : 2; break;
    case 2: side = targetParty ? 1 : 2; break;
    case 5: side = sourceParty ? 2 : 1; break;
    case 6: side = targetParty ? 2 : 1; break;
    case 3: {
        CameraView* camera = GetField0x3b0Value(game);
        if (camera) {
            cameraRotation = RotationMatrixY(GetField0x70(camera));
            Mat4x3_ApplyToVector(&command->offset_, &cameraRotation, &position);
        }
        break;
    }
    case 4:
    case 11: {
        if (!source || !target) return 1;
        GameObject* actor = game->GetGameObjectByIndex(target->actor_);
        if (!actor) return 1;
        int radius = actor->obj3D_.GetRadius();
        int rotation = actor->obj3D_.rotation_.y;
        if (command->mode_ == 4) command->offset_.z += radius >> 1;
        else command->offset_.z -= radius >> 1;
        if (command->heightMode_ == 1) {
            command->offset_.y = actor->obj3D_.GetHeight() >> 1;
            if (command->maximumHeight_ < command->offset_.y) command->offset_.y = command->maximumHeight_;
            else if (command->offset_.y < command->minimumHeight_) command->offset_.y = command->minimumHeight_;
        }
        actorRotation = RotationMatrixY(rotation);
        Mat4x3_ApplyToVector(&command->offset_, &actorRotation, &relative);
        Vector3fix actorPosition = actor->obj3D_.position_;
        Vector3fix_Add(&relative, &actorPosition, &relative);
        angle = fix32ReduceAngle0To2Pi(rotation + 0x3244);
        int* nextActor = actors;
        for (int i = 0; i < count; ++i, ++nextActor) {
            GameObject* object = game->GetGameObjectByIndex(*nextActor);
            if (object) {
                object->obj3D_.position_ = relative;
                if (object->obj3D_.unknown_0_ & 2) SetVecYFromValue02033874((Obj02033874*)object, angle);
                else StoreVec3AtField0x50((unsigned char*)object, 0, angle, 0);
            }
        }
        return 1;
    }
    case 7: {
        int references[12];
        DispatchByIndex021820bc(context, (int)lists, command->reference_, (int)references);
        GameObject* actor = game->GetGameObjectByIndex(references[0]);
        if (actor) { position = actor->obj3D_.position_; angle = actor->obj3D_.rotation_.y; }
        break;
    }
    case 10: {
        GameObject* actor = game->GetGameObjectByIndex(source->actor_);
        if (actor) {
            position = actor->obj3D_.position_;
            position.y += actor->obj3D_.GetHeight() >> 1;
            angle = actor->obj3D_.rotation_.y;
        }
        break;
    }
    case 8: {
        Vector3fix cameraTarget;
        Vector3fix cameraEye;
        Vector3fix direction;
        CameraView* camera = GetField0x3b0Value(game);
        if (!camera) return 1;
        cameraTarget = camera->target_;
        cameraEye = camera->eye_;
        Vector3fix_Subtract(&cameraEye, &cameraTarget, &direction);
        if (command->reference_ > 0 && command->reference_ < 4) {
            int offsets[3];
            int* values = offsets;
            values[0] = command->offset_.x;
            values[1] = command->offset_.y;
            values[2] = command->offset_.z;
            unsigned char axis = command->reference_ - 1;
            values[axis] = FIX32_MULTIPLY(values[axis], -0x1000);
            Vector3fix_Normalize(&direction, &direction);
            Vector3fixMultiplyScalar(&direction, values[axis], &direction);
            position.x = cameraEye.x + direction.x;
            position.y = cameraEye.y + direction.y;
            position.z = cameraEye.z + direction.z;
        } else {
            position.x = FIX32_MULTIPLY(direction.x, command->offset_.x) + cameraTarget.x;
            position.y = FIX32_MULTIPLY(direction.y, command->offset_.y) + cameraTarget.y;
            position.z = FIX32_MULTIPLY(direction.z, command->offset_.z) + cameraTarget.z;
        }
        break;
    }
    case 9:
        position = command->offset_;
        if (command->reverse_) angle = 0x3244;
        break;
    }
    if (side) {
        position.x = position.y = 0;
        switch (side) {
        case 1: position.z = 0x2800; angle = 0x3244; break;
        case 2: position.z = -0x2800; angle = 0; break;
        }
    }
    int* nextActor = actors;
    for (int i = 0; i < count; ++i, ++nextActor) {
        GameObject* actor = game->GetGameObjectByIndex(*nextActor);
        if (actor) {
            actor->obj3D_.position_ = position;
            if (actor->obj3D_.unknown_0_ & 2) SetVecYFromValue02033874((Obj02033874*)actor, angle);
            else StoreVec3AtField0x50((unsigned char*)actor, 0, angle, 0);
        }
    }
    return 1;
}
