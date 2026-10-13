#include <globaldefs.h>
#include "Memory/SafeAllocator.h"
#include "std_library_functions.h"

struct Element020de650 {
    unsigned int v[8];
};

struct Container020de650 {
    unsigned short count;
    unsigned char pad[10];
    struct Element020de650* base;
};

typedef int (*KeyFunc020de650)(struct Element020de650*);

struct Record020836e4 {
    unsigned int v[8];
};
static inline int KeyOf(Record020836e4* r) { return *(short*)((char*)r + 0x18); }

struct StructDE1D4;

struct EntryDf618 {
    void* name;
    void* desc;
    unsigned char data[0x18];
};

struct EntryDf6cc {
    void* name;
    void* desc;
    unsigned char data[0x20 - 8];
};

struct TableBits {
    unsigned short counter : 15;
    unsigned short flag : 1;
};

struct Table020def98 {
    unsigned short count;
    TableBits bits;
    unsigned short capacity;
    unsigned short used;
    unsigned char pad[4];
    Record020836e4* base;
};

int GetField0x18Short(void*);
struct Element020de650* BinarySearchByComparator020de650(struct Container020de650* c, int key, KeyFunc020de650 fn);
int CopyEntryAndAllocateStrings020df618(SafeAllocator* allocator, void* b, EntryDf618* dst, void** e, int unused, EntryDf618* src);
int CopyEntryAndIncrementCounter020df6cc(void* a, EntryDf6cc* b, int unused, EntryDf6cc* d);
void CopyStruct020836e4(Record020836e4* dst, Record020836e4* src);
void InitStruct020de1d4(struct StructDE1D4* p);

// USA: func_020def98
extern "C" ARM int func_020def98(SafeAllocator* allocator, Table020def98* table, Container020de650* src, short* keys, short count) {
    short key2;
    int key;
    int n;
    Record020836e4* dst;
    int total;
    int withData;
    short* kp;
    int i;
    int i2;
    short* jp;
    int j;
    int found;
    Element020de650* elem;
    void* strings;
    int filled;
    int cap;

    if (table == NULL || src == NULL || keys == NULL || (n = count) == 0) {
        return 0;
    }
    dst = table->base;
    if (src->base == NULL) {
        return 0;
    }
    if (allocator != NULL && dst == NULL) {
        total = 0;
        withData = 0;
        kp = keys;
        for (i = 0; i < n; i++, kp++) {
            key = *kp;
            if (key < 0) continue;
            Element020de650* e1 = BinarySearchByComparator020de650(src, key, (KeyFunc020de650)GetField0x18Short);
            if (e1 == NULL) continue;
            found = 0;
            jp = keys;
            for (j = 0; j < i; j++, jp++) {
                if (*jp == key) {
                    found = 1;
                    break;
                }
            }
            if (found) continue;
            total++;
            if (e1->v[0] != 0) withData++;
        }
        dst = (Record020836e4*)allocator->Allocate((total + n) << 5);
        table->count = total;
        table->bits.counter = withData;
        table->base = dst;
    }
    if (dst == NULL) {
        return 0;
    }
    table->capacity = count;
    table->used = table->bits.counter;
    strings = allocator != NULL ? (void*)(dst + count) : NULL;
    table->bits.counter = 0;
    filled = 0;
    kp = keys;
    for (i = 0; i < count; i++, kp++) {
        key2 = *kp;
        elem = NULL;
        if (key2 >= 0) {
            elem = BinarySearchByComparator020de650(src, key2, (KeyFunc020de650)GetField0x18Short);
        }
        if (elem == NULL) {
            if (allocator == NULL) {
                dst++;
                filled++;
            }
            continue;
        }
        if (allocator != NULL) {
            found = 0;
            for (j = 0; j < i; j++, jp++) {
                jp = keys;
                if (*jp == key2) {
                    found = 1;
                    break;
                }
            }
            if (found) continue;
            CopyEntryAndAllocateStrings020df618(allocator, table, (EntryDf618*)dst, &strings, (int)src, (EntryDf618*)elem);
        } else {
            CopyEntryAndIncrementCounter020df6cc(table, (EntryDf6cc*)dst, (int)src, (EntryDf6cc*)elem);
        }
        if (filled >= count) break;
        dst++;
        filled++;
    }
    table->count = filled;
    table->bits.flag = 0;
    if (allocator != NULL && table->count >= 2) {
        if (table != NULL && table->base != NULL && table->count != 0) {
            int hi = table->count - 1;
            int lo = 0;
            for (;;) {
                Record020836e4 tmp2;
                Record020836e4 tmp1;
                Record020836e4* p = table->base + lo;
                int lastSwap = -1;
                for (i2 = lo; i2 < hi; i2++, p++) {
                    if (KeyOf(p) > KeyOf(p + 1)) {
                        tmp1 = *p;
                        lastSwap = i2;
                        CopyStruct020836e4(p, p + 1);
                        CopyStruct020836e4(p + 1, &tmp1);
                    }
                }
                if (lastSwap == -1) break;
                hi = lastSwap;
                int lastBack = -1;
                Record020836e4* q = table->base + lastSwap;
                for (j = lastSwap; j > lo; j--) {
                    Record020836e4* prev = q - 1;
                    if (KeyOf(prev) > KeyOf(q)) {
                        tmp2 = *q;
                        lastBack = j;
                        CopyStruct020836e4(q, prev);
                        CopyStruct020836e4(prev, &tmp2);
                    }
                    q = prev;
                }
                if (lastBack == -1) break;
                lo = lastBack;
            }
        }
    }
    {
        Record020836e4* p;
        int idx;
        idx = table->count;
        p = table->base + idx;
        cap = table->capacity;
        for (; idx < cap; idx++, p++) {
            InitStruct020de1d4((StructDE1D4*)p);
        }
    }
    return 1;
}
