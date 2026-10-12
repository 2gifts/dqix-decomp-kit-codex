#include <globaldefs.h>
#include "std_library_functions.h"
#include "Memory/SafeAllocator.h"

struct Element020e0310 {
    short key;
    short pad;
    void *ptr;
};

struct Container020e0310;
typedef int (*KeyFunc020e0310)(struct Element020e0310 *);

struct List020e0280 {
    unsigned int count : 12;
    unsigned int rest : 20;
    struct Element020e0310 *entries;
    void *field8;
};

struct Element020e0310 *BinarySearch020e0310(struct Container020e0310 *c, int key, KeyFunc020e0310 fn);
short GetShortAt0x0020dfbd4(short *p);
char *AllocateStringCopy020da150(SafeAllocator *alloc, const char *s);
extern "C" int func_020e0280(struct List020e0280 *self, struct List020e0280 *src, unsigned char *outFlag, void *callback);
extern "C" const char *func_020dfbfc(struct List020e0280 *self, unsigned int offset, const char *fallback);

// USA: func_020e0028
extern "C" ARM int func_020e0028(struct List020e0280 *self, SafeAllocator *alloc, struct List020e0280 *src, void *aux, short *keys, unsigned short count) {
    short *key;
    int found;
    unsigned char flag;
    struct List020e0280 list;
    struct Element020e0310 *elem;
    struct Element020e0310 *entry;
    const char *name;
    int i;
    int n;

    if (alloc == NULL || src == NULL || aux == NULL || keys == NULL || count == 0) return 0;
    memset(self, 0, sizeof(*self));
    memset(&list, 0, sizeof(list));
    func_020e0280(&list, src, &flag, NULL);

    found = 0;
    key = keys;
    n = count;
    for (i = 0; i < n; i++, key++) {
        if (BinarySearch020e0310((struct Container020e0310 *)&list, *key, (KeyFunc020e0310)GetShortAt0x0020dfbd4) != NULL) {
            found++;
        }
    }

    self->entries = (struct Element020e0310 *)alloc->Allocate(found * 8);
    if (self->entries != NULL) {
        self->count = found;
        key = keys;
        entry = self->entries;
        for (i = 0; i < n; i++, key++) {
            elem = BinarySearch020e0310((struct Container020e0310 *)&list, *key, (KeyFunc020e0310)GetShortAt0x0020dfbd4);
            if (elem != NULL) {
                name = func_020dfbfc(&list, (unsigned int)elem->ptr, NULL);
                entry->key = elem->key;
                entry->ptr = AllocateStringCopy020da150(alloc, name);
                entry++;
            }
        }
    }

    if (found >= 2) {
        if (self != NULL) {
            int total = self->count;
            int idx;
            struct Element020e0310 *p;
            int high;
            int low;
            int lastSwap;
            if (self->entries != NULL && total != 0) {
                low = 0;
                high = total - 1;
                for (;;) {
                    idx = low;
                    p = self->entries + low;
                    i = -1;
                    for (; idx < high; idx++, p++) {
                        if (p->key > p[1].key) {
                            struct Element020e0310 tmp = *p;
                            p->key = p[1].key;
                            p->ptr = p[1].ptr;
                            p[1].key = tmp.key;
                            p[1].ptr = tmp.ptr;
                            i = idx;
                        }
                    }
                    if (i == -1) break;

                    high = i;
                    lastSwap = -1;
                    p = self->entries + high;
                    for (; i > low; i--) {
                        elem = p - 1;
                        if (elem->key > p->key) {
                            struct Element020e0310 tmp2 = *p;
                            p->key = elem->key;
                            p->ptr = elem->ptr;
                            elem->key = tmp2.key;
                            elem->ptr = tmp2.ptr;
                            lastSwap = i;
                        }
                        p = elem;
                    }
                    if (lastSwap == -1) break;
                    low = lastSwap;
                }
            }
        }
    }
    return 1;
}
