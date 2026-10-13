#include <globaldefs.h>
#include "Memory/SafeAllocator.h"
#include "std_library_functions.h"

struct Header020a8e9c {
    unsigned int count1 : 12;
    unsigned int count2 : 19;
    unsigned int flag : 1;
};

struct Obj020a8e9c {
    Header020a8e9c header;
    void* buf1;
    void* buf2;
};

struct Byte24_a9304 { unsigned char v[24]; };
struct Half6_a9304 { unsigned short v[6]; };

struct Struct020a9304 {
    unsigned short a;
    unsigned short b;
    unsigned short c;
    unsigned short d;
    unsigned short e;
    unsigned short f;
    unsigned int g;
    struct Byte24_a9304 arr1;
    unsigned int h;
    unsigned int i;
    unsigned char j;
    unsigned char k;
    struct Half6_a9304 arr2;
};

struct Raw64_020a8fd0 {
    unsigned int words[16];
};

struct U16Field0x0_020a8e80 {
    unsigned short field;
};

typedef int (*CmpFn020a9288)(void*);
typedef void (*IterFn020a8f70)(void*, void*);

struct Obj020a8f60;
struct Obj020a8f70;
struct Obj020a9288;

int GetElementsByteSize020a8f60(struct Obj020a8f60* obj);
int ForEachElement020a8f70(struct Obj020a8f70* obj, IterFn020a8f70 fn);
void* BinarySearchByComparator020a9288(struct Obj020a9288* obj, int key, CmpFn020a9288 fn);
struct Struct020a9304* CopyStruct020a9304(struct Struct020a9304* dst, struct Struct020a9304* src);
unsigned short GetU16At0x0(struct U16Field0x0_020a8e80* obj);

// USA: func_020a8fd0
extern "C" ARM void TODO_Name_020a8fd0(struct Obj020a8e9c* dst, SafeAllocator* allocator, struct Obj020a8e9c* src, void* cookie, short* keys, short count) {
    struct Obj020a8e9c tmp;
    struct Raw64_020a8fd0 swapBack;
    struct Raw64_020a8fd0 swapFwd;
    struct Struct020a9304* elems;
    struct Struct020a9304* p;
    struct Struct020a9304* prev;
    int i, j, found, last, rev, start, hi, k, n;
    char* out;
    short* keyp;
    void* found_p;

    if (allocator && src && cookie && keys) {
    if (count > 0) {
        memset(dst, 0, sizeof(struct Obj020a8e9c));
        memset(&tmp, 0, sizeof(struct Obj020a8e9c));
        if (src != 0) {
            memcpy(&tmp, src, 4);
            tmp.buf1 = (char*)src + 4;
            tmp.buf2 = (char*)src + (GetElementsByteSize020a8f60((struct Obj020a8f60*)&tmp) + 4);
            if (tmp.header.flag == 0) {
                ForEachElement020a8f70((struct Obj020a8f70*)&tmp, 0);
                tmp.header.flag = 1;
                src->header.flag = 1;
            }
        }

        keyp = keys;
        int cnt = count;
        for (i = found = 0; i < cnt; i++) {
            found_p = BinarySearchByComparator020a9288((struct Obj020a9288*)&tmp, *keyp, (CmpFn020a9288)GetU16At0x0);
            if (found_p != 0) {
                found++;
            }
            keyp++;
        }

        dst->buf1 = allocator->Allocate(found * 64);
        if (dst->buf1 != 0) {
            dst->header.count1 = found;
            out = (char*)dst->buf1;
            keyp = keys;
            for (j = 0; j < cnt; j++) {
                found_p = BinarySearchByComparator020a9288((struct Obj020a9288*)&tmp, *keyp, (CmpFn020a9288)GetU16At0x0);
                if (found_p != 0) {
                    memcpy(out, found_p, 64);
                    out += 64;
                }
                keyp++;
            }
        }

        if (found >= 2 && dst != 0) {
            n = dst->header.count1;
            if (dst->buf1 != 0 && n != 0) {
            hi = n - 1;
            start = 0;
            for (;;) {
                elems = (struct Struct020a9304*)dst->buf1;
                last = -1;
                p = elems + start;
                for (i = start; i < hi; i++) {
                    if (p[0].a > p[1].a) {
                        last = i;
                        swapFwd = *(struct Raw64_020a8fd0*)&p[0];
                        CopyStruct020a9304(&p[0], &p[1]);
                        CopyStruct020a9304(&p[1], (struct Struct020a9304*)&swapFwd);
                    }
                    p++;
                }
                rev = -1;
                if (last == -1) {
                    break;
                }
                elems = (struct Struct020a9304*)dst->buf1;
                p = elems + last;
                for (k = last; k > start; k--) {
                    prev = p - 1;
                    if (prev->a > p->a) {
                        rev = k;
                        swapBack = *(struct Raw64_020a8fd0*)p;
                        CopyStruct020a9304(p, prev);
                        CopyStruct020a9304(prev, (struct Struct020a9304*)&swapBack);
                    }
                    p = prev;
                }
                if (rev == -1) {
                    break;
                }
                start = rev;
            }
            }
        }
    }
    }
}
