#include <globaldefs.h>

struct SearchStruct { char pad0[0xfc0]; int fieldFC0; };
struct SearchStruct0202c1a4 { char pad[0x1038]; signed char arr[16]; };
struct QList0205e1e4;
struct QList0205e18c;
struct QNode0205e18c { char pad[0x14]; short id; unsigned short flags; };
struct Struct0205e3ec;
struct BitArrayObj0205e830;
struct Entry0205e57c { unsigned short low : 3; };
struct Elem0205e57c { char pad[0x10]; unsigned char state; unsigned char seq; char pad2[0x14 - 0x12]; };
struct Slot0205e57c { unsigned char state; unsigned char seq; char pad[0x14 - 0x2]; };
struct Shared0205e57c { unsigned char seq; char pad[3]; int count4; int count8; };

extern "C" void* func_0202ae18(void);
void* GetSubstructAt0x7c0(void* base);
struct QNode0205e18c* DequeueNode(struct QList0205e1e4* list);
void CopyStruct0205e3ec(struct Struct0205e3ec* dst, struct Struct0205e3ec* src);
void EnqueueNode(struct QList0205e18c* list, struct QNode0205e18c* node);
int TestBitBySignedByteIndex(struct SearchStruct* base, int index);
signed char GetSearchStructCurrentArrEntry(struct SearchStruct0202c1a4* base);
struct Entry0205e57c* GetEntryBySignedByteIndex(struct SearchStruct* base, int index);
int TestBitInArray0x1524(struct BitArrayObj0205e830* obj, int index);

extern struct Shared0205e57c data_02108748;

// USA: func_0205e57c
extern "C" ARM void TODO_Name_0205e57c(void* obj) {
    struct SearchStruct* base = (struct SearchStruct*)func_0202ae18();
    if (data_02108748.seq >= 0xfa) {
        data_02108748.seq = 0;
    }
    if (base->fieldFC0 != 0) {
        return;
    }
    struct Elem0205e57c* arr = (struct Elem0205e57c*)GetSubstructAt0x7c0(base);
    for (int i = 0; i < 5; i++) {
        if (arr[i].state != 0xff) {
            data_02108748.count4 += 0x10;
        }
        arr[i].state = 0xff;
    }
    for (int i = 0; i < 5; i++) {
        int allFree;
        struct QNode0205e18c* node;
        struct Slot0205e57c* slot = (struct Slot0205e57c*)((char*)arr + 0x10);
        if (slot[i].state != 0xff) {
            continue;
        }
        node = *(struct QNode0205e18c**)((char*)obj + 0x150c);
        if (node == NULL) {
            continue;
        }
        if (node->flags & 1) {
            allFree = 1;
            struct Elem0205e57c* other = (struct Elem0205e57c*)GetSubstructAt0x7c0(func_0202ae18());
            for (int j = 0; j < 5; j++) {
                if (other[j].state != 0xff) {
                    allFree = 0;
                    break;
                }
            }
            if (allFree) {
                node->flags &= ~1;
            }
        }
        if (node->flags & 1) {
            continue;
        }
        char* lst2 = (char*)obj + 0x10c;
        DequeueNode((struct QList0205e1e4*)(lst2 + 0x1400));
        CopyStruct0205e3ec((struct Struct0205e3ec*)&slot[i], (struct Struct0205e3ec*)node);
        int next = data_02108748.seq + 1;
        data_02108748.seq = next;
        arr[i].seq = (unsigned char)next;
        EnqueueNode((struct QList0205e18c*)((char*)obj + 0x1500), node);
    }
    for (int id = 0; id < 4; id++) {
        if (!TestBitBySignedByteIndex(base, id)) {
            continue;
        }
        if (id == GetSearchStructCurrentArrEntry((struct SearchStruct0202c1a4*)base)) {
            continue;
        }
        struct Entry0205e57c* entry = GetEntryBySignedByteIndex(base, id);
        if (entry == NULL) {
            continue;
        }
        if (entry->low == 0) {
            continue;
        }
        short key = (short)id;
        char* lst = (char*)obj;
        lst += 0x118;
        struct Slot0205e57c* entrySlots = (struct Slot0205e57c*)((char*)entry + 0x10);
        for (int k = 0; k < 5; k++) {
            GetSearchStructCurrentArrEntry((struct SearchStruct0202c1a4*)base);
            if (entrySlots[k].state == 0xff) {
                continue;
            }
            if (!TestBitInArray0x1524((struct BitArrayObj0205e830*)obj, entrySlots[k].state)) {
                continue;
            }
            struct QNode0205e18c* n = DequeueNode((struct QList0205e1e4*)((char*)obj + 0x1500));
            if (n == NULL) {
                continue;
            }
            CopyStruct0205e3ec((struct Struct0205e3ec*)n, (struct Struct0205e3ec*)&entrySlots[k]);
            n->id = key;
            EnqueueNode((struct QList0205e18c*)(lst + 0x1400), n);
            data_02108748.count8 += 0x10;
        }
    }
}
