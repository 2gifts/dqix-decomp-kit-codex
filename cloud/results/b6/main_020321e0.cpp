#include <globaldefs.h>
#include "Graphics/Vector.h"
extern "C" void func_02031118(const Vector3fix*, void*);
// USA: func_020321e0
extern "C" ARM void func_020321e0(const Vector3fix* input, void* context, int angle, const Vector3fix* pivot, int squaredRange) {
    Vector3fix position = *input;
    if (angle) {
        Vector3fix displacement;
        Vector3fix_Subtract(input, pivot, &displacement);
        fix32_t dx = displacement.x;
        fix32_t dz = displacement.z;
        dx = FIX32_MULTIPLY(dx, dx);
        dz = FIX32_MULTIPLY(dz, dz);
        if (dx + dz > squaredRange) return;
        Matrix4x3 matrix = RotationMatrixY(fix32ReduceAngle0To2Pi(-angle));
        Mat4x3_ApplyToVector(&displacement, &matrix, &displacement);
        Vector3fix_Add(&displacement, pivot, &position);
    }
    func_02031118(&position, context);
}
