#include <globaldefs.h>

extern "C" float _ffltu(unsigned int v);
extern "C" float _fflt(int v);
extern "C" float _fdiv(float a, float b);
extern "C" float _fsub(float a, float b);
extern "C" float _fmul(float a, float b);
extern "C" int _ffix(float v);
extern "C" int fix32_Divide(unsigned int numerHi, unsigned int denomLo);
extern "C" int fix32sin(int v);
extern "C" int fix32abs(int v);

static inline int FxMul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

struct Obj02015cd0 {
    char pad0[0xe];
    short field0e;
    unsigned short field10;
    char pad1[0x18 - 0x12];
    unsigned char state;
};

// USA: func_02015cd0
extern "C" ARM int TODO_Name_02015cd0(struct Obj02015cd0* p) {
    int a;
    int t;
    int u;

    if (p->state == 0) {
        unsigned short n = p->field10;
        if (_ffltu((unsigned)n) <= _fdiv(_fflt(49140), 4096.0f)) {
            t = n * n << 12;
            p->field0e = 0x4800 + FxMul(fix32_Divide(-0x800, 589536), t);
            p->field10++;
        } else if (_ffltu((unsigned)n) <= _fdiv(_fflt(98280), 4096.0f)) {
            t = _ffix(_fmul(4096.0f, _fsub(_ffltu((unsigned)n), _fdiv(_fflt(49140), 4096.0f))));
            a = 0x1000 - fix32_Divide(t, 98280);
            t = FxMul(fix32_Divide(51471, 98280), t);
            if (t > 25722) {
                t = 25722;
            }
            u = FxMul(fix32sin(t), 257);
            p->field0e = 0x4000 + FxMul(u, a);
            p->field10++;
        } else {
            return 1;
        }
    } else if (p->state == 1) {
        if (p->field10 == 0) {
            a = 25722 - p->field0e;
            t = FxMul(a, 819);
            p->field0e = p->field0e + t;
            if (fix32abs(t) < 4) {
                p->field0e = 25722;
                p->field10++;
                return 1;
            }
        } else {
            return 1;
        }
    }

    return 0;
}
