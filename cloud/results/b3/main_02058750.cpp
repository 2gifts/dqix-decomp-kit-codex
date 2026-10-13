#include <globaldefs.h>
#include <Memory/SignedAllocator.h>

struct S02055080;

struct Count02058750 { char pad[8]; unsigned char count1; unsigned char count2; };
struct Track02058750 { char pad[0xb0]; SignedAllocatorList list1; SignedAllocatorList list2; };
struct Entry02058750 { unsigned int time; unsigned int value; };
struct Node02058750 { struct Entry02058750* entry; };
struct Obj02058750 {
    char pad_00[4];
    unsigned int time;
    char pad_08[0x20];
    struct S02055080* track;
    char pad_2c[4];
    float scale;
    float field_34;
    float field_38;
    float field_3c;
    float field_40;
    char pad_44[0x20];
    unsigned short index1;
    unsigned short index2;
};

extern void* GetSubField0x18(struct S02055080* p);

// USA: func_02058750
extern "C" ARM void func_02058750(struct Obj02058750* self) {
    struct Count02058750* counts = (struct Count02058750*)GetSubField0x18(self->track);

    if (counts->count1 > 1 && counts->count1 > self->index1) {
        struct Node02058750* previous = (struct Node02058750*)((struct Track02058750*)self->track)->list1.GetNthElement(self->index1);
        if (previous && previous->entry->time < self->time) {
            self->index1++;
            if (self->index1 >= counts->count1) {
                self->field_34 = 0;
                self->field_38 = 0;
                self->field_3c = 0;
            } else {
                struct Node02058750* next = (struct Node02058750*)((struct Track02058750*)self->track)->list1.GetNthElement(self->index1);
                if (next) {
                    struct Entry02058750* prev = previous->entry;
                    struct Entry02058750* nxt = next->entry;
                    unsigned int pr = prev->value & 0x1f;
                    unsigned int nr = nxt->value & 0x1f;
                    unsigned int ng = nxt->value & 0x3e0;
                    unsigned int pg = prev->value & 0x3e0;
                    unsigned int pb = prev->value & 0x7c00;
                    unsigned int nb = nxt->value & 0x7c00;
                    self->field_34 = (float)(nr - pr) / (float)(int)(next->entry->time - previous->entry->time);
                    self->field_38 = (float)((ng >> 5) - (pg >> 5)) / (float)(int)(next->entry->time - previous->entry->time);
                    self->field_3c = (float)((nb >> 10) - (pb >> 10)) / (float)(int)(next->entry->time - previous->entry->time);
                }
            }
        }
    }

    if (counts->count2 > 1 && counts->count2 > self->index2) {
        struct Node02058750* previous = (struct Node02058750*)((struct Track02058750*)self->track)->list2.GetNthElement(self->index2);
        if (previous && (float)previous->entry->time * self->scale < (float)self->time) {
            self->index2++;
            if (self->index2 >= counts->count2) {
                self->field_40 = 0;
            } else {
                struct Node02058750* next = (struct Node02058750*)((struct Track02058750*)self->track)->list2.GetNthElement(self->index2);
                if (next) {
                    struct Entry02058750* prev = previous->entry;
                    float a = (float)prev->time * self->scale;
                    struct Entry02058750* nxt = next->entry;
                    float b = (float)nxt->time * self->scale;
                    self->field_40 = (float)(int)(nxt->value - prev->value) / (b - a);
                }
            }
        }
    }
}
