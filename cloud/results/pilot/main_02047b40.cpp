#include <globaldefs.h>
#include "Memory/SafeAllocator.h"
#include "std_library_functions.h"

extern "C" void func_0204719c(void* obj, int flag);
extern "C" void func_02047f70(void* obj, void* buf, unsigned int size, SafeAllocator* alloc);

struct Obj02047ed4;
struct Struct020478bc;

void DuplicateBufferIfFlagged02047ed4(struct Obj02047ed4* obj, int* outSize, void* buf, unsigned int size, SafeAllocator* alloc);
void RecomputeField0x7c(struct Struct020478bc* o, int newIndex);

struct Array02048090 {
    int count;
    void* buffer;
};

void AllocateArray02048090(struct Array02048090* obj, int count, SafeAllocator* alloc);

struct PackedTexture {
    int size;
    unsigned short info;
    unsigned short placement;
};

struct PackedGroup {
    unsigned char width;
    unsigned char height;
    unsigned char count;
    unsigned char pad3;
    PackedTexture* textures;
};

struct Triple {
    int a;
    int b;
    int c;
};

struct TripleSet {
    unsigned int count;
    Triple* items;
};

struct FileGroup {
    unsigned short width;
    unsigned short height;
    unsigned int count;
};

struct FileTexture {
    unsigned short x;
    unsigned short y;
    unsigned short wlog;
    unsigned short hlog;
};

struct Obj02047b40 {
    char pad0[4];
    unsigned short mode;
    unsigned short groupCount;
    PackedGroup* groups;
    TripleSet* sets;
    char pad10[4];
    Array02048090 frames;
    char pad1c[0x84 - 0x1c];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char restBits : 6;
};

// USA: func_02047b40
extern "C" ARM void func_02047b40(struct Obj02047b40* obj, unsigned char* src, SafeAllocator* alloc) {
    unsigned char* data;
    unsigned int tail;
    int flag;
    PackedGroup* group;
    PackedTexture* tex;
    int g;
    int k;
    int n;
    FileGroup* fg;
    FileTexture* ft;
    unsigned int w;
    unsigned int h;
    unsigned int shift;
    unsigned int size;
    if (src == 0) {
        return;
    }
    flag = obj->flag0;
    if (flag) {
        if (flag) {
            func_0204719c(obj, flag);
        }
        obj->flag0 = 0;
    }
    memcpy(&obj->groupCount, src, 2);
    memcpy(&obj->mode, src + 2, 2);
    data = src + 4;
    group = obj->groups = (PackedGroup*)alloc->Allocate(obj->groupCount * 8);
    for (g = 0; g < obj->groupCount; g++, group++) {
        fg = (FileGroup*)data;
        data += 8;
        group->width = fg->width;
        group->height = fg->height;
        group->count = fg->count;
        tex = group->textures = (PackedTexture*)alloc->Allocate(group->count * 8);
        for (k = 0; k < group->count; k++) {
            ft = (FileTexture*)data;
            h = 8 << ft->wlog;
            shift = 0;
            data += 8;
            w = 8 << ft->hlog;
            if (obj->mode == 3) {
                shift = 1;
                size = h * w;
                DuplicateBufferIfFlagged02047ed4((Obj02047ed4*)obj, &tex->size, data, size >> 1, alloc);
            } else {
                size = h * w;
                DuplicateBufferIfFlagged02047ed4((Obj02047ed4*)obj, &tex->size, data, size, alloc);
            }
            data += size >> shift;
            tex->info = (ft->hlog << 4) | ft->wlog;
            tex->placement = ft->x | ((group->height - ft->y - w) << 8);
            tex++;
        }
    }
    memcpy(&tail, data, 4);
    data += 4;
    if (tail <= 0x10) {
        func_02047f70(obj, data, 0x20, alloc);
    } else if (tail <= 0x100) {
        func_02047f70(obj, data, 0x200, alloc);
    } else {
        func_02047f70(obj, data, tail, alloc);
    }
    data += tail * 2;
    data += 0x1e;
    memcpy(&obj->frames, data, 4);
    data += 4;
    AllocateArray02048090(&obj->frames, obj->frames.count, alloc);
    if (obj->frames.count > 0) {
        for (n = 0; n < obj->frames.count; n++) {
            memcpy((char*)obj->frames.buffer + n * 0x1e, data, 0x1e);
            data += 0x1e;
        }
        TripleSet* set = obj->sets = (TripleSet*)alloc->Allocate(obj->frames.count * 8);
        for (int m = 0; m < obj->frames.count; m++, set++) {
            memcpy(set, data, 4);
            data += 4;
            set->items = (Triple*)alloc->Allocate(set->count * 12);
            Triple* out = set->items;
            int* pa = (int*)data;
            data += set->count * 4;
            int* pb = (int*)data;
            data += set->count * 4;
            int* pc = (int*)data;
            data += set->count * 4;
            for (unsigned int i = 0; i < set->count; i++) {
                out->a = *pa;
                out->b = *pb;
                out->c = *pc;
                pa++;
                pb++;
                pc++;
                out++;
            }
        }
    }
    obj->flag0 = 1;
    obj->flag1 = 1;
    if (obj->sets != 0) {
        if (obj->sets->count == 0 && obj->frames.count == 1) {
            RecomputeField0x7c((Struct020478bc*)obj, 0);
        } else {
            RecomputeField0x7c((Struct020478bc*)obj, 1);
        }
    } else {
        RecomputeField0x7c((Struct020478bc*)obj, 0);
    }
}
