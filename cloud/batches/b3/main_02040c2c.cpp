#include <globaldefs.h>

struct Vector3i {
    int x;
    int y;
    int z;
    Vector3i& operator=(const Vector3i& rhs);
};

struct Vec3_020406f8 { unsigned int v[3]; };
struct Node020406f8;
struct Vec3_020407c4 { unsigned int v[3]; };
struct Self020407c4;
struct Obj02041298;
struct Obj0204117c;
struct Vec3s32_020c3030 { int x; int y; int z; };
struct HeadNode02046b24;

struct GameState {
    static GameState* GetInstance();
};

struct LightingManager {
    static LightingManager* GetInstance();
    char pad_00[0x44];
    unsigned short f44;
    unsigned short f46;
};

struct Object3DChild {
    char pad_00[0x54];
    void* f54;
};

struct Object3D {
    char pad_00[0x8];
    struct Object3DChild* child08;
    char pad_0c[0x44 - 0x0c];
    struct Vector3i v44;
    struct Vector3i v50;
    char pad_5c[0xac - 0x5c];
    unsigned short flags_ac;
    char pad_ae[0xc0 - 0xae];
    int limit_c0;

    void SetDiffuseColor(unsigned int value);
    void AdvanceEffects();
};

struct Container02040c2c {
    char pad_00[0x1f];
    unsigned char mode;
    char pad_20[0x48 - 0x20];
    struct Vector3i v48;
};

struct TargetA02040c2c {
    char pad_00[0x4];
    struct Vec3_020406f8 v4;
    char pad_10[0x2a - 0x10];
    unsigned short f2a;
    char pad_2c[0x5c - 0x2c];
    unsigned int flags5c;
    char pad_60[0x88 - 0x60];
    int f88;
};

struct Obj02040c2c {
    unsigned int flags;
    int timer;
    char pad_08[0x4];
    struct Container02040c2c* cont;
    struct Object3D* d;
    struct TargetA02040c2c* a;
    struct Object3D* b;
};

struct GameObj3b0 {
    char pad_00[0x10];
    struct Vector3i v10;
};

struct Ov017State {
    char pad_0000[0x36fc];
    struct HeadNode02046b24** headNode;
};

extern "C" void func_0203c818();
extern "C" struct Ov017State* func_ov017_0218b5b0();
extern "C" void func_02040f48(struct Obj02040c2c* self, void* vec);
extern "C" int rand(void);
extern "C" void Vector3fix_Subtract(void* a, void* b, void* out);
extern "C" void Vector3fix_Add(void* a, void* b, void* out);
extern "C" int GetField0x78(void* p);
extern "C" void GetPtrField0x58(void* p);

int GetHeadNodeIdOrMinusOne(struct HeadNode02046b24** head);
void DispatchNearestActiveNode0204117c(struct Obj0204117c* obj, struct Vec3s32_020c3030* refPoint);
void AdvanceAndAccumulate02041298(struct Obj02041298* obj, int value);
void SelectVec3FromSources020406f8(struct Vec3_020406f8* out, struct Node020406f8* node);
void FillVec3Default020407c4(struct Vec3_020407c4* out, struct Self020407c4* self);
struct GameObj3b0* GetField0x3b0Value(struct GameState* gs);
void Vector3fixDivideScalar(const struct Vector3i* a, int b, struct Vector3i* out);

// USA: func_02040c2c
extern "C" ARM void TODO_Name_02040c2c(struct Obj02040c2c* self, int p1, int p2, unsigned int p3) {
    struct Vec3_020406f8 v30;
    struct Vec3_020406f8 v24;
    struct Vec3_020407c4 tmpB;
    struct Vector3i tmpC;
    struct Vec3_020406f8 tmpA;

    if (self->a != 0) {
        func_0203c818();
        struct LightingManager* lm = LightingManager::GetInstance();
        if (lm != 0) {
            self->a->f2a = lm->f44;
        }
        if (GetHeadNodeIdOrMinusOne(func_ov017_0218b5b0()->headNode) != 4) {
            if (!(self->flags & 1) && !(self->flags & 0x20000)) {
                if (self->cont != 0 && self->cont->mode != 0) {
                    struct TargetA02040c2c* a = self->a;
                    if (!(a->flags5c & 1)) {
                        unsigned int lim = a->f88;
                        if (lim > (unsigned int)self->timer) {
                            v30 = a->v4;
                            switch (self->cont->mode) {
                            case 7:
                            case 8:
                                func_02040f48(self, &v30);
                                break;
                            case 9:
                            case 10:
                                DispatchNearestActiveNode0204117c((struct Obj0204117c*)self, (struct Vec3s32_020c3030*)&v30);
                                break;
                            }
                            if (self->flags & 8) {
                                self->timer = 0;
                            } else {
                                self->timer = rand() % 2000 + 2000;
                            }
                        }
                    }
                }
            }
        }
    } else if (self->b != 0) {
        AdvanceAndAccumulate02041298((struct Obj02041298*)self->b, p2);
        if (self->b->child08 != 0 && self->b->child08->f54 != 0) {
            if (LightingManager::GetInstance() != 0) {
                self->b->SetDiffuseColor(LightingManager::GetInstance()->f46);
            }
        }
        if (GetHeadNodeIdOrMinusOne(func_ov017_0218b5b0()->headNode) != 4) {
            if (!(self->flags & 1) && !(self->flags & 0x20000)) {
                if (self->cont != 0 && self->cont->mode != 0) {
                    struct Object3D* b = self->b;
                    if (!(b->flags_ac & 1)) {
                        unsigned int limb = b->limit_c0;
                        if (limb > (unsigned int)self->timer) {
                            switch (self->cont->mode) {
                            case 7:
                            case 8:
                                func_02040f48(self, &b->v44);
                                break;
                            case 9:
                            case 10:
                                DispatchNearestActiveNode0204117c((struct Obj0204117c*)self, (struct Vec3s32_020c3030*)&b->v44);
                                break;
                            }
                            if (self->flags & 8) {
                                self->timer = 0;
                            } else {
                                self->timer = rand() % 2000 + 2000;
                            }
                        }
                    }
                }
            }
        }
    }

    if (self->d != 0) {
        SelectVec3FromSources020406f8(&tmpA, (struct Node020406f8*)self);
        v24 = tmpA;
        FillVec3Default020407c4(&tmpB, (struct Self020407c4*)self);
        struct Vector3i* pB = (struct Vector3i*)&tmpB;
        v24.v[1] += 0x1000;
        if (self->a != 0) {
            struct GameObj3b0* r7 = GetField0x3b0Value(GameState::GetInstance());
            GetPtrField0x58(r7);
            float fx = (float)GetField0x78(r7);
            fx *= 3.0f;
            int r6 = (int)fx;
            Vector3fix_Subtract(&r7->v10, &v24, &tmpC);
            Vector3fixDivideScalar(&tmpC, r6, &tmpC);
            Vector3fix_Add(&v24, &tmpC, &v24);
        }
        Vector3fix_Add(&v24, &self->cont->v48, &v24);
        self->d->v44 = *(struct Vector3i*)&v24;
        self->d->v50 = *pB;
        self->d->AdvanceEffects();
    }
    self->flags &= ~0x20000u;
}
