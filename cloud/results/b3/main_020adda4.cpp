#include <globaldefs.h>
#include "Graphics/Vector.h"

struct ObjFields020adda4 {
    char pad0[0x44];
    Vector3i f44;
    char pad50[0xae - 0x50];
    short fae;
    char padb0[0x164 - 0xb0];
    Vector3i f164;
    int f170;
    Vector3i f174;
    int f180;
};

// USA: func_020adda4
extern "C" ARM void func_020adda4(ObjFields020adda4* obj) {
    Vector3i v = obj->f44;
    Vector3i scaled;
    short angle = obj->fae;

    Vector3i dir;
    int zval = fix32cos(angle);
    dir.x = fix32sin(angle);
    dir.y = 0;
    dir.z = zval;

    Vector3i dirPrev;
    int zvalPrev = fix32cos(obj->f170);
    dirPrev.x = fix32sin(obj->f170);
    dirPrev.y = 0;
    dirPrev.z = zvalPrev;

    Vector3i dirNext;
    int zvalNext = fix32cos(obj->f180);
    dirNext.x = fix32sin(obj->f180);
    dirNext.y = 0;
    dirNext.z = zvalNext;

    Vector3fix_Normalize(&dir, &dir);
    Vector3fix_Normalize(&dirPrev, &dirPrev);
    Vector3fix_Normalize(&dirNext, &dirNext);

    scaled = dir;
    Vector3fixMultiplyScalar(&scaled, 0xe66, &scaled);

    Vector3i sum1;
    Vector3fix_Subtract(&v, &scaled, &sum1);

    Vector3fix_Subtract(&sum1, &obj->f164, &dirPrev);
    Vector3fix_Normalize(&dirPrev, &dirPrev);
    scaled.x = -dirPrev.x;
    scaled.y = -dirPrev.y;
    scaled.z = -dirPrev.z;
    Vector3fixMultiplyScalar(&scaled, 0xe66, &scaled);
    Vector3fix_Add(&sum1, &scaled, &obj->f164);
    obj->f170 = fix32_Atan2(dirPrev.x, dirPrev.z);
    obj->f170 = fix32ReduceAngle0To2Pi(obj->f170);

    scaled = dirPrev;
    Vector3fixMultiplyScalar(&scaled, 0xe66, &scaled);
    Vector3i sum2;
    Vector3fix_Subtract(&obj->f164, &scaled, &sum2);
    Vector3fix_Subtract(&sum2, &obj->f174, &dirNext);
    Vector3fix_Normalize(&dirNext, &dirNext);
    scaled.x = -dirNext.x;
    scaled.y = -dirNext.y;
    scaled.z = -dirNext.z;
    Vector3fixMultiplyScalar(&scaled, 0xe66, &scaled);
    Vector3fix_Add(&sum2, &scaled, &obj->f174);
    obj->f180 = fix32_Atan2(dirNext.x, dirNext.z);
    obj->f180 = fix32ReduceAngle0To2Pi(obj->f180);
}
