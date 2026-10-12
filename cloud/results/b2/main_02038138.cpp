#include <globaldefs.h>
#include "GameState/GameState.h"

struct Vec3 { int x; int y; int z; };

struct Bytes02033b88;
struct Foo02033b58;
struct Obj02033b68;
struct Obj02033834;
struct ShortField0x22e_020968c4;
struct Container0201e8fc;

struct Box0201e8fc {
    char pad0[0x18];
    short angle;
};

struct Cursor02114e54 {
    char pad0[0x30];
    int x;
    int y;
    char pad38[0x40 - 0x38];
    int count;
};

struct Neighbor02038138 {
    char pad0[0x224];
    unsigned char active;
    unsigned char valid;
};

struct Obj02038138 {
    char pad0[0x44];
    struct Vec3 pos;
    char pad50[0xbe - 0x50];
    unsigned char field0xbe;
    char padbf[0xc0 - 0xbf];
    unsigned char field0xc0;
    char padc1[0x250 - 0xc1];
    unsigned char dir;
    char pad251;
    unsigned char hasTarget;
    char pad253[2];
    signed char lastDir;
    short lastAngle;
};

extern "C" void* func_02012fe4(void);

int GetField0x3b0Value(GameState* battleStruct);
int IsFlag0x40Set(unsigned short* flags);
int IsFlag0x20Set(unsigned short* flags);
int IsFlag0x10Set(unsigned short* flags);
int IsFlag0x80Set(unsigned short* flags);
void* GetField4328_0218d268(void* obj);
void SelectCoordsByFlag0x24(unsigned char* obj, int* out1, int* out2);
int IsMainBrightnessTransitionActive(GameResources* resources);
int SetByte0xbeShiftPrev(struct Bytes02033b88* p, int val);
void SetByteIfChanged02033b68(struct Obj02033b68* obj, int newVal);
void SetByteSavingPrevious(struct Foo02033b58* p, unsigned char v);
int GetField0x70(void* obj);
int fix32ReduceAngle0To2Pi(int angle);
struct Box0201e8fc* FindBoxContainingPoint0201e8fc(struct Container0201e8fc* obj, struct Vec3* pt);
int fix32SignedAngleDistance(int a, int b);
int fix32abs(int x);
void SetVecYByMode02033834(struct Obj02033834* obj, int arg);
int GetShortAt0x22e(struct ShortField0x22e_020968c4* p);

extern unsigned short data_02114e30;
extern struct Cursor02114e54 data_02114e54;
extern int data_020e780c[];

// USA: func_02038138
extern "C" ARM void func_02038138(struct Obj02038138* obj) {
    int field3b0 = GetField0x3b0Value(GameState::GetInstance());
    GameResources* res = func_ov017_0218b5b0();
    char* ctx = (char*)func_02012fe4();
    int mode = 0;

    if (IsFlag0x40Set(&data_02114e30) && IsFlag0x20Set(&data_02114e30)) {
        obj->dir = 12;
        mode = 1;
    } else if (IsFlag0x40Set(&data_02114e30) && IsFlag0x10Set(&data_02114e30)) {
        obj->dir = 13;
        mode = 1;
    } else if (IsFlag0x80Set(&data_02114e30) && IsFlag0x20Set(&data_02114e30)) {
        obj->dir = 14;
        mode = 1;
    } else if (IsFlag0x80Set(&data_02114e30) && IsFlag0x10Set(&data_02114e30)) {
        obj->dir = 15;
        mode = 1;
    } else if (IsFlag0x40Set(&data_02114e30)) {
        obj->dir = 8;
        mode = 1;
    } else if (IsFlag0x80Set(&data_02114e30)) {
        obj->dir = 9;
        mode = 1;
    } else if (IsFlag0x20Set(&data_02114e30)) {
        obj->dir = 10;
        mode = 1;
    } else if (IsFlag0x10Set(&data_02114e30)) {
        obj->dir = 11;
        mode = 1;
    }

    struct Neighbor02038138* other = (struct Neighbor02038138*)GetField4328_0218d268(res);
    if (data_02114e54.count >= 5 && other->valid != 0 && other->active != 0) {
        int px;
        int py;
        mode = 2;
        SelectCoordsByFlag0x24((unsigned char*)&data_02114e54, &px, &py);
        Vector3fix a;
        Vector3fix b;
        a.x = px << 12;
        a.y = py << 12;
        a.z = 0;
        px = data_02114e54.x;
        py = data_02114e54.y;
        b.x = px << 12;
        b.y = py << 12;
        b.z = 0;
        if (Vector3fix_Distance(&a, &b) < 0x8000) {
            mode = 0;
        }
    }

    if (obj->hasTarget == 0 || IsMainBrightnessTransitionActive(res) != 0) {
        mode = 0;
    }

    if (mode == 0) {
        if (obj->dir >= 8) {
            obj->dir -= 8;
        }
        if (obj->hasTarget != 0 && obj->field0xbe == 1) {
            SetByte0xbeShiftPrev((struct Bytes02033b88*)obj, 0);
        }
        obj->lastDir = -1;
    } else if (mode == 1) {
        if (obj->field0xc0 == 0 || obj->field0xc0 == 0xa) {
            SetByteIfChanged02033b68((struct Obj02033b68*)obj, 1);
        } else {
            SetByteSavingPrevious((struct Foo02033b58*)obj, 1);
        }
        int base = data_020e780c[obj->dir - 8];
        int angle = fix32ReduceAngle0To2Pi(base + GetField0x70((void*)field3b0));
        if (obj->lastDir == obj->dir) {
            angle = obj->lastAngle;
        } else {
            struct Box0201e8fc* box = FindBoxContainingPoint0201e8fc((struct Container0201e8fc*)(ctx + 0x6c), &obj->pos);
            if (box != 0) {
                if (fix32abs(fix32SignedAngleDistance(angle, box->angle)) < 0xc91) {
                    obj->lastDir = obj->dir;
                    obj->lastAngle = box->angle;
                } else {
                    short alt = fix32ReduceAngle0To2Pi(box->angle + 0x3244);
                    if (fix32abs(fix32SignedAngleDistance(angle, alt)) < 0xc91) {
                        obj->lastDir = obj->dir;
                        obj->lastAngle = alt;
                        angle = obj->lastAngle;
                    }
                }
            } else {
                obj->lastDir = -1;
            }
        }
        SetVecYByMode02033834((struct Obj02033834*)obj, angle);
    } else if (mode == 2) {
        if (obj->field0xc0 == 0) {
            SetByteIfChanged02033b68((struct Obj02033b68*)obj, 1);
        } else {
            SetByteSavingPrevious((struct Foo02033b58*)obj, 1);
        }
        if (other->active != 0) {
            SetVecYByMode02033834((struct Obj02033834*)obj, GetShortAt0x22e((struct ShortField0x22e_020968c4*)other));
        }
        obj->lastDir = -1;
    }
}
