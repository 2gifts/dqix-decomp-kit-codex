#include <globaldefs.h>

struct Struct_0205c570;
extern "C" int _Z26GetGlobalField0x1c020421a0v(void);
extern "C" int _Z26GetActiveScaledSum0205c570P15Struct_0205c570(struct Struct_0205c570* s);
void SetField0x1e20(void* obj, void* value);
void EmitScaledField0x440(void* obj, int a, int b);
void InitSelfPointer(unsigned char* base);
extern "C" int _Z35CheckGlobalObjState2AndInit0205cde8Ph(unsigned char* p);
extern "C" void func_02045f3c(void* ctx, void* ptr, int a, int b, int c, int d, int e, int flag, int g, int h);

struct Foo02042fcc {
    short a0;
    short a2;
    short a4;
    short a6;
    int a8;
    int ac;
    int a10;
    int a14;
    short a18;
};

struct Container0205a3d0;
struct Entry0205a3d0 {
    char pad0[4];
    unsigned short f4;
    unsigned short f6;
    char pad8[0xd];
    unsigned char f15;
};
extern "C" void _Z26SetEntryFlag2ByKey0205a370P17Container0205a3d0i(struct Container0205a3d0* c, int key);
extern "C" struct Entry0205a3d0* _Z27FindEntryByHalfword0205a3d0P17Container0205a3d0i(struct Container0205a3d0* c, int key);
extern "C" void func_0205ae8c(void* obj);

// USA: func_0205c96c
extern "C" ARM void func_0205c96c(unsigned char* obj, int flag) {
    if (obj[0x233] != 0 && obj[0x232] != 0) {
        unsigned char* ctx = (unsigned char*)_Z26GetGlobalField0x1c020421a0v();
        int bound50 = *(int*)(obj + 0x50);
        int sum = _Z26GetActiveScaledSum0205c570P15Struct_0205c570((struct Struct_0205c570*)(obj + 0x1c));
        int poolVal = 0x7fff;
        if (obj[0x234] == 0) poolVal = 0x3def;

        if (flag == 0) {
            struct Foo02042fcc* foo = (struct Foo02042fcc*)(obj + 0xb4);
            foo->a18 = poolVal;
            SetField0x1e20((void*)ctx, (void*)foo);
            EmitScaledField0x440((void*)(ctx + 0x19e0), 1, 1);
            InitSelfPointer(ctx);
        }

        if (_Z35CheckGlobalObjState2AndInit0205cde8Ph(obj) != 0) {
            ctx[0x19b0] = 1;

            int a = *(int*)(obj + 0x7c);
            int b = *(int*)(obj + 0x84);
            struct Foo02042fcc* foo2 = (struct Foo02042fcc*)(obj + 0xb4);
            int c = foo2->a2;
            int sb = a * b;
            int acc = c + 8;
            int d = foo2->a0;
            int loopEnd = sb + a;
            int r7val = d + 0xc;
            unsigned short* base120 = (unsigned short*)(obj + 0x120);
            int accInit = acc;

            for (; sb < loopEnd; sb++) {
                int special = poolVal;
                if (sb == bound50 && bound50 == sum) special = 0xf0a;
                unsigned short idx = *(unsigned short*)(obj + 0x220 + sb * 2);
                func_02045f3c((void*)ctx, base120 + idx, r7val, acc, special, 0xc, 0x14, flag, 1, 0x11);
                acc += *(int*)(obj + 0xd8);
            }

            if ((obj[0xb0] | obj[0xb1]) != 0) {
                int e = *(int*)(obj + 0x88);
                int f = *(int*)(obj + 0x8c);
                int g = *(int*)(obj + 0xd4);
                int h = r7val - 0x10;
                void* p2d8 = *(void**)(ctx + 0x2d8);
                int ii = *(int*)(obj + 0xd8);
                int jj = accInit - 2;
                int val2 = f;
                int val1 = e * g + h;
                val2 = val2 * ii + jj;
                void* ctx2 = *(void**)(ctx + 0x2e0);
                unsigned short ev = *(unsigned short*)(ctx + 0x2e4);

                if (p2d8 != NULL && ctx2 != NULL) {
                    _Z26SetEntryFlag2ByKey0205a370P17Container0205a3d0i((struct Container0205a3d0*)ctx2, ev);
                    struct Entry0205a3d0* entry = _Z27FindEntryByHalfword0205a3d0P17Container0205a3d0i((struct Container0205a3d0*)ctx2, ev);
                    if (entry != NULL) entry->f15 |= 8;
                    struct Entry0205a3d0* entry2 = _Z27FindEntryByHalfword0205a3d0P17Container0205a3d0i((struct Container0205a3d0*)ctx2, ev);
                    if (entry2 != NULL) {
                        entry2->f4 = val1;
                        entry2->f6 = val2;
                    }
                    func_0205ae8c(p2d8);
                }
            }
        }
    }
}
