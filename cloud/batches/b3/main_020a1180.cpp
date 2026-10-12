#include <globaldefs.h>
#include "Memory/SafeAllocator.h"

struct Variant02030b0c {
    int tag;
    union {
        int i;
        float f;
    } u;
};
ARM int GetIntFromVariant02030b0c(struct Variant02030b0c* p);

struct Struct02030b7c {
    int field0;
    void* field4;
};
ARM void* GetField4IfField0Zero(struct Struct02030b7c* s);

struct BitfieldRecord020a1380 {
    unsigned int f0 : 9;   // bits 0-8
    unsigned int f1 : 9;   // bits 9-17
    unsigned int f2 : 9;   // bits 18-26
    unsigned int f3 : 3;   // bits 27-29
    unsigned int f4 : 2;   // bits 30-31
    int next;   // 0x4
    int prev;   // 0x8
};
ARM void ResetBitfieldRecord(struct BitfieldRecord020a1380* rec);

ARM int StringLength(const char* s);

struct Elem020a1568 {
    unsigned int w0;
    unsigned int w1;
    unsigned int w2;
};
struct Array020a1568 {
    struct Elem020a1568* base;
    unsigned short capacity;
    unsigned short count;
};
ARM void AppendElement020a1568(struct Array020a1568* arr, struct Elem020a1568* src);

extern "C" int sprintf(char* dst, const char* fmt, ...);
extern char data_020f1914;

struct Global02109d94_020a1180 {
    unsigned char flags;         // 0x0
    char pad1;                   // 0x1
    unsigned short validCount;   // 0x2
    struct Array020a1568* arr;   // 0x4
    SafeAllocator* alloc;        // 0x8
    short* validIds;             // 0xc
};
extern struct Global02109d94_020a1180 data_02109d94;

struct VariantArgs020a1180 {
    struct Variant02030b0c v0;   // 0x00
    struct Variant02030b0c v1;   // 0x08
    struct Variant02030b0c v2;   // 0x10
    struct Variant02030b0c v3;   // 0x18
    struct Variant02030b0c v4;   // 0x20
    struct Struct02030b7c v5;    // 0x28
};

// USA: func_020a1180
ARM int BuildAndAppendBitfieldRecord020a1180(struct VariantArgs020a1180* args) {
    struct BitfieldRecord020a1380 rec;
    ResetBitfieldRecord(&rec);
    ResetBitfieldRecord(&rec);

    int v0 = GetIntFromVariant02030b0c(&args->v0);
    rec.f0 = v0;

    struct Global02109d94_020a1180* g = &data_02109d94;
    short* validIds = g->validIds;

    if (validIds != 0 && g->validCount != 0) {
        int found = 0;
        int i = 0;
        while (i < g->validCount) {
            if (rec.f0 == (unsigned int)validIds[i]) {
                found = 1;
                break;
            }
            i++;
        }
        if (!found) {
            return 1;
        }
    }

    rec.f1 = GetIntFromVariant02030b0c(&args->v1);
    rec.f2 = GetIntFromVariant02030b0c(&args->v2);
    rec.f3 = GetIntFromVariant02030b0c(&args->v3);

    if ((data_02109d94.flags & 1) == 0) {
        if (rec.f3 == 0 || rec.f3 == 1) return 1;
    }
    if ((data_02109d94.flags & 2) == 0) {
        if (rec.f3 == 2) return 1;
    }
    if ((data_02109d94.flags & 4) == 0) {
        if (rec.f3 == 7) return 1;
    }
    if ((data_02109d94.flags & 8) == 0) {
        if (rec.f3 == 3 || rec.f3 == 4 || rec.f3 == 5 || rec.f3 == 6) return 1;
    }

    rec.f4 = GetIntFromVariant02030b0c(&args->v4);

    char* src = (char*)GetField4IfField0Zero(&args->v5);
    int len = StringLength(src);
    rec.next = (int)data_02109d94.alloc->Allocate(len + 1);
    sprintf((char*)rec.next, &data_020f1914, src);
    ((char*)rec.next)[len] = 0;
    AppendElement020a1568(data_02109d94.arr, (struct Elem020a1568*)&rec);

    return 1;
}
