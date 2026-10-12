#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Vector.h"
#include "Combat/FormationPosition.h"

int CheckSubstructFlag0x100(unsigned char* obj);
struct S_10088;
int IsFlag10088Set(struct S_10088* obj);
void ClearSubstructFlag0x4(unsigned char* obj);
int GetSubstructByte0x56(unsigned char* obj);
int GetByteWithFallback(unsigned char* obj);
int GetSubstructByte0x1c(unsigned char* obj);
struct Obj020494c0;
void DispatchTargetByte020494c0(struct Obj020494c0* obj);
extern "C" fix32_t func_02031468(const Vector3fix* start, const Vector3fix* end,
                                 const Vector3fix* point, Vector3fix* closest);

struct Barrier02107850 {
    Vector3fix start;
    Vector3fix end;
    fix32_t radius;
    unsigned char enabled;
};
extern struct Barrier02107850 data_02107850;
extern struct Barrier02107850 data_02107850_dup;

struct WaitTimes0204956c {
    int v[3];
};
extern const Vector3fix data_020e7b3c;
extern const Vector3fix data_020e7b30;
extern const struct WaitTimes0204956c data_020e7b54;
extern const char data_020f01a4[];

struct Orbit0204956c {
    unsigned char pad0[0x20];
    unsigned int flags;
    unsigned char pad24[0x4e - 0x24];
    unsigned char direction;
    unsigned char stepsLeft;
    short speed;
    unsigned short waitTimer;
    unsigned char cycle;
    unsigned char pad55;
    unsigned char active;
};

class Orbiter0204956c : public GameObject
{
public:
    Orbit0204956c* orbit_;
};

static inline Vec2_0216f74c GetFormationPosition(const int& cell) {
    return func_ov000_0216f74c(const_cast<int*>(&cell));
}

// USA: func_0204956c
extern "C" ARM void func_0204956c(Orbiter0204956c* obj) {
    Orbit0204956c* orbit = obj->orbit_;
    GameState* state;
    int phase;
    GameObject* target;
    int dt;
    int idx;
    struct Barrier02107850* barrier;

    if (!(orbit->flags & 4)) return;
    if (orbit->active == 0 || CheckSubstructFlag0x100((unsigned char*)obj) ||
        IsFlag10088Set((struct S_10088*)obj)) {
        ClearSubstructFlag0x4((unsigned char*)obj);
        return;
    }
    if (!GetSubstructByte0x56((unsigned char*)obj)) return;

    state = GameState::GetInstance();
    orbit = obj->orbit_;
    if (orbit->waitTimer != 0) {
        dt = state->GetEffectiveDeltaTime();
        if (dt < orbit->waitTimer) {
            orbit->waitTimer -= dt;
            return;
        }
        orbit->waitTimer = 0;
    }

    if (orbit->stepsLeft == 0) {
        phase = obj->obj3D_.unknown_4_;
        phase = phase + GetByteWithFallback((unsigned char*)obj) + orbit->cycle;
        idx = GetByteWithFallback((unsigned char*)obj);
        if (idx == 0xff) {
            ClearSubstructFlag0x4((unsigned char*)obj);
            return;
        }
        target = state->GetCombatantByIndex(idx);
        if (target == NULL) {
            ClearSubstructFlag0x4((unsigned char*)obj);
            return;
        }
        barrier = &data_02107850_dup;
        if (data_02107850.enabled != 0) {
            Vector3fix start = barrier->start;
            Vector3fix end = barrier->end;
            Vector3fix point = obj->obj3D_.position_;
            Vector3fix closest;
            fix32_t dist = func_02031468(&start, &end, &point, &closest);
            if (dist < barrier->radius + (obj->obj3D_.GetRadius() >> 1) + 0x2000) {
                ClearSubstructFlag0x4((unsigned char*)obj);
                return;
            }
        }
        {
            Vec2_0216f74c pos = GetFormationPosition(GetSubstructByte0x1c((unsigned char*)obj));
            Vector3fix dest = data_020e7b3c;
            dest.x = pos.x;
            dest.z = pos.y;
            Vector3fix targetPos = target->obj3D_.position_;
            Vector3fix selfPos = obj->obj3D_.position_;
            Vector3fix toSelf;
            Vector3fix toTarget;
            BCFG::AnimationRecord* record;
            int odd;
            Vector3fix_Subtract(&selfPos, &dest, &toSelf);
            Vector3fix_Subtract(&targetPos, &dest, &toTarget);
            Vector3fix_Normalize(&toSelf, &toSelf);
            Vector3fix_Normalize(&toTarget, &toTarget);
            odd = phase % 2;
            if (odd != 0) {
                if (Vector3fix_InnerProduct(&toSelf, &toTarget) > 0) {
                    orbit->direction = 1;
                } else {
                    orbit->direction = 0;
                }
            } else {
                Vector3fix cross;
                Vector3fix_CrossProduct(&toSelf, &toTarget, &cross);
                if (cross.y > 0) {
                    orbit->direction = 2;
                } else {
                    orbit->direction = 3;
                }
            }
            orbit->stepsLeft = odd + 1;
            record = obj->obj3D_.GetLoadedAnimationRecord(data_020f01a4);
            if (record != NULL) {
                unsigned int frames = (float)fix32_Divide(record->endTime - record->startTime, record->frameRate) / 4096.0f;
                orbit->speed = 0xc00 / (frames * 17);
            }
            obj->obj3D_.MaybeSetRegularAnimation(data_020f01a4, 1);
            DispatchTargetByte020494c0((struct Obj020494c0*)obj);
            return;
        }
    }

    idx = GetByteWithFallback((unsigned char*)obj);
    if (idx == 0xff) {
        ClearSubstructFlag0x4((unsigned char*)obj);
        return;
    }
    target = state->GetCombatantByIndex(idx);
    if (target == NULL) {
        ClearSubstructFlag0x4((unsigned char*)obj);
        return;
    }
    {
        Vector3fix selfPos = obj->obj3D_.position_;
        Vector3fix targetPos = target->obj3D_.position_;
        Vector3fix dir;
        Vector3fix_Subtract(&targetPos, &selfPos, &dir);
        Vector3fix up = data_020e7b30;
        Vector3fix step;
        switch (orbit->direction) {
        case 0:
            Vector3fix_Normalize(&dir, &dir);
            break;
        case 1:
            Vector3fixMultiplyScalar(&dir, -0x1000, &dir);
            Vector3fix_Normalize(&dir, &dir);
            break;
        case 2:
            Vector3fix_Normalize(&dir, &dir);
            Vector3fix_CrossProduct(&dir, &up, &dir);
            Vector3fixMultiplyScalar(&dir, -0x1000, &dir);
            break;
        case 3:
            Vector3fix_Normalize(&dir, &dir);
            Vector3fix_CrossProduct(&dir, &up, &dir);
            break;
        default:
            ClearSubstructFlag0x4((unsigned char*)obj);
            break;
        }
        Vector3fixMultiplyScalar(&dir, orbit->speed * state->GetEffectiveDeltaTime(), &step);
        Vector3fix_Add(&selfPos, &step, &selfPos);
        obj->obj3D_.position_ = selfPos;
    }
    if (obj->obj3D_.HasAnimationStopped()) {
        if (--orbit->stepsLeft == 0) {
            int base = obj->obj3D_.unknown_4_;
            base = base + GetByteWithFallback((unsigned char*)obj) + orbit->cycle;
            struct WaitTimes0204956c waits = data_020e7b54;
            orbit->waitTimer = waits.v[base % 3];
            orbit->cycle++;
            return;
        }
        obj->obj3D_.StopCurrentAnimation();
        obj->obj3D_.MaybeSetRegularAnimation(data_020f01a4, 1);
        DispatchTargetByte020494c0((struct Obj020494c0*)obj);
        return;
    }
    if (obj->obj3D_.activeAnimationRecord_ != NULL &&
        strcmp(obj->obj3D_.activeAnimationRecord_->name, data_020f01a4) != 0) {
        ClearSubstructFlag0x4((unsigned char*)obj);
    }
}
