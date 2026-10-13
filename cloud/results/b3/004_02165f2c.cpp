#include <globaldefs.h>

struct Elem02165f2c {
    char pad[2];
    unsigned short kind : 2;
    unsigned short wide : 1;
    unsigned short rest : 13;
    char pad4[4];
    struct Elem02165f2c* next;
};

struct Seq3 { unsigned char v[3]; };
struct Seq4 { unsigned char v[4]; };
struct Table9 { char pad[6]; struct Seq3 seq; };

struct Obj02165f2c {
    struct Elem02165f2c* arr[2];
    struct Elem02165f2c* head;
    char pad[0x48 - 0xc];
    short counts[2];
    short count;
    char pad4e[2];
    unsigned char lo : 6;
    unsigned char second : 1;
    unsigned char first : 1;
};

extern struct Obj02165f2c* data_ov004_0217101c;
extern struct Table9 data_ov004_0216ff98;
extern struct Seq4 data_ov004_0216ffa1;

// USA: func_ov004_02165f2c
extern "C" ARM void func_ov004_02165f2c(void) {
    struct Obj02165f2c* obj = data_ov004_0217101c;
    if (obj->first == 1) {
        struct Elem02165f2c* base = obj->arr[0];
        obj->head = base;
        obj->count = obj->counts[0];
        short n = obj->counts[0];
        struct Seq3 a = data_ov004_0216ff98.seq;
        struct Seq4 b = data_ov004_0216ffa1;
        unsigned char* p = a.v;
        struct Elem02165f2c* prev = 0;
        if (obj->second == 1) {
            base = obj->arr[1];
            obj->head = base;
            obj->count = obj->counts[1];
            n = obj->counts[1];
            p = b.v;
        }
        int t;
        struct Elem02165f2c* e;
        short i;
        for (; (t = *p) != 0xff; p++) {
            e = base;
            for (i = 0; i < n; i++, e++) {
                if (e->next != 0) continue;
                if (e->wide && t == 1) {
                    if (e->kind != 0 && e->kind != t) continue;
                } else if (e->kind != t) {
                    continue;
                }
                if (prev) prev->next = e; else obj->head = e;
                prev = e;
            }
        }
    } else if (obj->first == 0) {
        struct Elem02165f2c* e = obj->arr[0];
        obj->head = e;
        obj->count = obj->counts[0];
        short n = obj->counts[0];
        if (obj->second == 1) {
            e = obj->arr[1];
            obj->head = e;
            obj->count = obj->counts[1];
            n = obj->counts[1];
        }
        short i;
        for (i = 0; i < n - 1; i++) {
            struct Elem02165f2c* nx = e + 1;
            e->next = nx;
            e = nx;
        }
    }
}
