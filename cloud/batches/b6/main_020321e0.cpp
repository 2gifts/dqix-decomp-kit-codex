#include <globaldefs.h>
#include "Graphics/Vector.h"
extern "C" void func_02031118(const Vector3fix*, void*);
// USA: func_020321e0
extern "C" ARM void func_020321e0(const Vector3fix* input, void* context, int angle, const Vector3fix* pivot, int squaredRange) {
    Vector3fix position = *input;
    if (angle) {
        Vector3fix displacement;
        Vector3fix_Subtract(input, pivot, &displacement);
        long long zProduct = (long long)displacement.z * displacement.z;
        long long xProduct = (long long)displacement.x * displacement.x;
        int zSquared = (zProduct + 0x800) >> 12;
        int xSquared = (xProduct + 0x800) >> 12;
        if (squaredRange < zSquared + xSquared) return;
        Matrix4x3 matrix = RotationMatrixY(fix32ReduceAngle0To2Pi(-angle));
        Mat4x3_ApplyToVector(&displacement, &matrix, &displacement);
        Vector3fix_Add(&displacement, pivot, &position);
    }
    func_02031118(&position, context);
}
