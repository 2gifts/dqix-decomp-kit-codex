#include <globaldefs.h>

struct Vec3_020adda4 { int x, y, z; };

extern "C" short _Z8fix32cosi(int x);
extern "C" short _Z8fix32sini(int x);
extern "C" void Vector3fix_Normalize(struct Vec3_020adda4* v, struct Vec3_020adda4* out);
extern "C" void _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(struct Vec3_020adda4* in, int scale, struct Vec3_020adda4* out);
extern "C" void Vector3fix_Subtract(struct Vec3_020adda4* a, struct Vec3_020adda4* b, struct Vec3_020adda4* out);
extern "C" void Vector3fix_Add(struct Vec3_020adda4* a, struct Vec3_020adda4* b, struct Vec3_020adda4* out);
extern "C" void _ZN8Vector3iaSERKS_(int* dst, int* src);
extern "C" int fix32_Atan2(int y, int x);
extern "C" int _Z22fix32ReduceAngle0To2Pii(int a);

struct ObjFields020adda4 {
    char pad0[0x44];
    Vec3_020adda4 f44;
    char pad50[0xae - 0x50];
    short fae;
    char padb0[0x164 - 0xb0];
    Vec3_020adda4 f164;
    int f170;
    Vec3_020adda4 f174;
    int f180;
};

// USA: func_020adda4
extern "C" ARM void func_020adda4(ObjFields020adda4* obj) {
    Vec3_020adda4 v = obj->f44;
    Vec3_020adda4 scaled;
    short angle = obj->fae;

    Vec3_020adda4 dir;
    int zval = 0;
    zval = zval + _Z8fix32cosi(angle);
    dir.x = _Z8fix32sini(angle);
    dir.y = 0;
    dir.z = zval;

    Vec3_020adda4 dirPrev;
    int zvalPrev = _Z8fix32cosi(obj->f170);
    dirPrev.x = _Z8fix32sini(obj->f170);
    dirPrev.y = 0;
    dirPrev.z = zvalPrev;

    Vec3_020adda4 dirNext;
    int zvalNext = _Z8fix32cosi(obj->f180);
    dirNext.x = _Z8fix32sini(obj->f180);
    dirNext.y = 0;
    dirNext.z = zvalNext;

    Vector3fix_Normalize(&dir, &dir);
    Vector3fix_Normalize(&dirPrev, &dirPrev);
    Vector3fix_Normalize(&dirNext, &dirNext);

    _ZN8Vector3iaSERKS_((int*)&scaled, (int*)&dir);
    _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(&scaled, 0xe66, &scaled);

    Vec3_020adda4 sum1;
    Vector3fix_Subtract(&v, &scaled, &sum1);

    Vector3fix_Subtract(&sum1, &obj->f164, &dirPrev);
    Vector3fix_Normalize(&dirPrev, &dirPrev);
    scaled.x = -dirPrev.x;
    scaled.y = -dirPrev.y;
    scaled.z = -dirPrev.z;
    _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(&scaled, 0xe66, &scaled);
    Vector3fix_Add(&sum1, &scaled, &obj->f164);
    obj->f170 = fix32_Atan2(dirPrev.x, dirPrev.z);
    obj->f170 = _Z22fix32ReduceAngle0To2Pii(obj->f170);

    _ZN8Vector3iaSERKS_((int*)&scaled, (int*)&dirPrev);
    _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(&scaled, 0xe66, &scaled);
    Vec3_020adda4 sum2;
    Vector3fix_Subtract(&obj->f164, &scaled, &sum2);
    Vector3fix_Subtract(&sum2, &obj->f174, &dirNext);
    Vector3fix_Normalize(&dirNext, &dirNext);
    scaled.x = -dirNext.x;
    scaled.y = -dirNext.y;
    scaled.z = -dirNext.z;
    _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(&scaled, 0xe66, &scaled);
    Vector3fix_Add(&sum2, &scaled, &obj->f174);
    obj->f180 = fix32_Atan2(dirNext.x, dirNext.z);
    obj->f180 = _Z22fix32ReduceAngle0To2Pii(obj->f180);
}
