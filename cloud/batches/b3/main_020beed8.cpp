#include <globaldefs.h>

struct Req020beed8 { unsigned int f0; unsigned char f4; unsigned char pad5[2]; unsigned char f7; };

struct Flags0x110_020beed8 { unsigned int pad0 : 6; unsigned int b6 : 1; };
struct Outer020beeac;
struct Tween020c01ec;
struct Tween020c0260;
struct Obj020bcc4c;

void* AttachOrRefreshEntry(void** self, int index, int value);
void InitActionHandlers020bffc4(void* entry, unsigned int arg);
void ResetField14cAndClearFlags020beeac(struct Outer020beeac* p);
void ResetTween020c01ec(struct Tween020c01ec* t);
void RetargetTween020c0204(struct Tween020c0260* t, int end, int duration);
int AcquireRefAt0x120(void* obj, int mode, void* p);
void ReleaseRefAt0x120(void* obj);
void SetBitForListEntry020bcc4c(struct Obj020bcc4c* obj, int index, int value);
extern "C" int func_020bc9d0(void* obj, int a1, void* a2, unsigned int a3, unsigned short a4, int a5, void* fp, void* a7);
extern "C" void func_020bf520();

// USA: func_020beed8
extern "C" ARM int TODO_Name_020beed8(void** self, struct Req020beed8* req, int index, int value, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int a8, unsigned int a9) {
    unsigned char* entry = (unsigned char*)AttachOrRefreshEntry(self, index, value);
    if (entry == NULL) {
        return 0;
    }
    InitActionHandlers020bffc4(entry, req->f0);
    if (((int (*)(void*, unsigned int))*(void**)(entry + 0x164))(entry, req->f0) == 0) {
        ResetField14cAndClearFlags020beeac((struct Outer020beeac*)entry);
        return 0;
    }
    *(unsigned int*)(entry + 0x160) = (unsigned int)(((unsigned long long)*(unsigned short*)(entry + 0xc4) * a5) / 1000);
    if (*(unsigned int*)(entry + 0x160) != 0 && *(unsigned char*)(entry + 0xc0) == 2) {
        *(unsigned int*)(entry + 0x110) |= 0x10;
    } else {
        *(unsigned int*)(entry + 0x110) &= ~0x10;
    }
    *(unsigned int*)(entry + 0x114) = 4;
    *(unsigned int*)(entry + 0x110) &= ~0x20;
    *(unsigned int*)(entry + 0x110) &= ~0x2;
    *(unsigned int*)(entry + 0x118) = 0;
    *(unsigned int*)(entry + 0x110) &= ~0x4;
    *(unsigned int*)(entry + 0x110) &= ~0x8;
    *(unsigned int*)(entry + 0x11c) = 0;
    *(unsigned int*)(entry + 0x134) = a6;
    *(unsigned int*)(entry + 0x138) = a7;
    *(unsigned int*)(entry + 0x13c) = a8;
    *(unsigned int*)(entry + 0x140) = a9;
    *(unsigned int*)(entry + 0x144) = a4;
    *(unsigned int*)(entry + 0x15c) = 0;
    *(unsigned int*)(entry + 0x154) = req->f4;
    *(unsigned int*)(entry + 0x158) = 0x7f;
    ResetTween020c01ec((struct Tween020c01ec*)(entry + 0xe8));
    RetargetTween020c0204((struct Tween020c0260*)(entry + 0xe8), 0x7f00, 1);
    int r5;
    if (*(unsigned char*)(entry + 0xc0) != 0) {
        if (*(unsigned char*)(entry + 0xc0) == 1 || *(unsigned char*)(entry + 0xc0) == 2) {
            r5 = 1;
        }
    } else {
        r5 = 0;
    }
    int mode = *(unsigned char*)(entry + 0xc2);
    if (req->f7 & 1) {
        mode = 2;
    }
    if (mode > *(unsigned char*)(entry + 0x124)) {
        mode = *(unsigned char*)(entry + 0x124);
    }
    ((struct Flags0x110_020beed8*)(entry + 0x110))->b6 = (mode == 1);
    if (AcquireRefAt0x120(entry, mode, entry + 0x126) == 0) {
        ((void (*)(void*))*(void**)(entry + 0x168))(entry);
        ResetField14cAndClearFlags020beeac((struct Outer020beeac*)entry);
        return 0;
    }
    unsigned int q = (*(unsigned int*)(entry + 0x130) * mode) / *(unsigned char*)(entry + 0x124);
    if (func_020bc9d0(entry, r5, *(void**)(entry + 0x12c), q, *(unsigned short*)(entry + 0xc6), 4, (void*)func_020bf520, entry) == 0) {
        ReleaseRefAt0x120(entry);
        ((void (*)(void*))*(void**)(entry + 0x168))(entry);
        ResetField14cAndClearFlags020beeac((struct Outer020beeac*)entry);
        return 0;
    }
    if (mode == 2) {
        SetBitForListEntry020bcc4c((struct Obj020bcc4c*)entry, 0, 0);
        SetBitForListEntry020bcc4c((struct Obj020bcc4c*)entry, 1, 0x7f);
    }
    return 1;
}
