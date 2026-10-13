#include <globaldefs.h>
#include "Memory/SafeAllocator.h"

extern "C" void __clear(void* dst, int size);
int CopyEntryAndAllocateStrings020df618(SafeAllocator* allocator, void* b, struct EntryDf618* dst, void** e, int unused, struct EntryDf618* src);

struct BitFieldDf374 {
    unsigned short counter : 15;
    unsigned short flag : 1;
};

struct EntryDf618 {
    void* name;
    void* desc;
    unsigned int mask_slot : 4;
    unsigned int bit_slot : 5;
    unsigned int extra : 23;
    unsigned char data[0x14];
};

struct TableDf374 {
    unsigned short count;
    unsigned char pad[0xa];
    EntryDf618* entries;
};

struct OutDf374 {
    unsigned short count;
    BitFieldDf374 bits;
    unsigned short saved_count;
    unsigned short saved_named;
    unsigned char pad[4];
    EntryDf618* entries;
};

// USA: func_020df374
extern "C" ARM int TODO_Name_020df374(SafeAllocator* allocator, OutDf374* out, TableDf374* in, unsigned char* maskBytes, int maskCount, unsigned char* listBytes, int listCount) {
    unsigned int mask;
    unsigned int bits[2];
    EntryDf618* entry;
    EntryDf618* dst;
    void* cursor;
    int total;
    int named;
    int count;
    int maskIndex;
    int listIndex;
    int countIndex;
    int i;
    int outIndex;

    if (allocator == NULL || out == NULL || in == NULL) {
        return 0;
    }
    if ((maskBytes == NULL || maskCount == 0) && (listBytes == NULL || listCount == 0)) {
        return 0;
    }
    entry = in->entries;
    dst = out->entries;
    if (entry == NULL) {
        return 0;
    }

    mask = 0;
    __clear(bits, 8);
    if (maskBytes == NULL) {
        mask = ~0u;
    } else {
        for (maskIndex = 0; maskIndex < maskCount; maskIndex++) {
            mask |= 1u << *maskBytes++;
        }
    }
    if (listBytes == NULL) {
        bits[0] = ~0u;
        bits[1] = ~0u;
    } else {
        for (listIndex = 0; listIndex < listCount; listIndex++) {
            bits[listIndex >> 5] |= 1u << (*listBytes++ & 0x1f);
        }
    }

    count = in->count;
    if (allocator != NULL && dst == NULL) {
        total = 0;
        named = 0;
        entry = in->entries;
        for (countIndex = 0; countIndex < count; countIndex++, entry++) {
            if (mask & (1u << entry->mask_slot)) {
                if (bits[0] & (1u << (entry->bit_slot & 0x1f))) {
                    total++;
                    if (entry->name != NULL) {
                        named++;
                    }
                }
            }
        }
        dst = (EntryDf618*)allocator->Allocate((total + named) << 5);
        out->count = total;
        out->bits.counter = named;
        out->entries = dst;
    }

    if (dst == NULL) {
        return 0;
    }

    total = out->count;
    out->saved_count = total;
    out->saved_named = out->bits.counter;
    cursor = (allocator != NULL) ? (void*)((char*)dst + (total << 5)) : NULL;
    out->bits.counter = 0;

    outIndex = 0;
    entry = in->entries;
    for (i = 0; i < count; i++, entry++) {
        if (mask & (1u << entry->mask_slot)) {
            if (bits[0] & (1u << (entry->bit_slot & 0x1f))) {
                CopyEntryAndAllocateStrings020df618(allocator, out, dst, &cursor, (int)in, entry);
                if (outIndex >= total) {
                    break;
                }
                dst++;
                outIndex++;
            }
        }
    }

    out->count = outIndex;
    out->bits.flag = 0;
    return 1;
}
