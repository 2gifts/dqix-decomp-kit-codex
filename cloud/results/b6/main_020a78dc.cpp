#include <globaldefs.h>
#include "GameState/GameState.h"

struct Vec3 { int x; int y; int z; };
struct Bytes02033b88;
struct Obj02033b68;
struct Foo02033b58;
struct Obj02033834;
struct Container0201e8fc;
struct ShortField0x22e_020968c4;

struct Box0201e8fc {
    char pad0[0x18];
    short angle;
};

struct Field4328 {
    char pad0[0x220];
    unsigned char x0;
    unsigned char y0;
    unsigned char x1;
    unsigned char y1;
    unsigned char active;
};

struct Input02114e54 {
    char pad0[0x40];
    int count;
};

struct Actor020a78dc {
    char pad0[0x44];
    struct Vec3 pos;
    char pad50[0xbe - 0x50];
    unsigned char fbe;
    char padbf;
    unsigned char fc0;
    char padc1[0x134 - 0xc1];
    int mode;
    char pad138[0x154 - 0x138];
    unsigned char f154;
    char pad155[0x158 - 0x155];
    unsigned char dir;
    char pad159[0x15d - 0x159];
    signed char lockedDir;
    short lockedAngle;
};

struct AngleTable { int v[8]; };

int GetField0x3b0Value(GameState*);
extern "C" char* func_02012fe4();
struct Field4328* GetField4328_0218d268(void*);
int IsFlag0x40Set(unsigned short*);
int IsFlag0x20Set(unsigned short*);
int IsFlag0x10Set(unsigned short*);
int IsFlag0x80Set(unsigned short*);
int IsField0Null(void** p);
void SetByte0xbeShiftPrev(struct Bytes02033b88*, int);
void SetByteIfChanged02033b68(struct Obj02033b68*, int);
void SetByteSavingPrevious(struct Foo02033b58*, unsigned char);
int GetField0x70(void*);
int fix32ReduceAngle0To2Pi(int);
struct Box0201e8fc* FindBoxContainingPoint0201e8fc(struct Container0201e8fc*, struct Vec3*);
int fix32SignedAngleDistance(int, int);
int fix32abs(int);
void SetVecYByMode02033834(struct Obj02033834*, int);
short GetShortAt0x22e(struct ShortField0x22e_020968c4*);

extern struct AngleTable data_020e90a8;
extern unsigned short data_02114e30;
extern struct Input02114e54 data_02114e54;

// USA: func_020a78dc
extern "C" ARM void func_020a78dc(struct Actor020a78dc* self) {
    char* world;
    void* camera;
    char* ov;
    struct Field4328* f;
    void** list;

    camera = (void*)GetField0x3b0Value(GameState::GetInstance());
    ov = (char*)func_ov017_0218b5b0();
    list = *(void***)(ov + 0x36fc);
    world = func_02012fe4();
    f = GetField4328_0218d268(ov);
    struct AngleTable table = data_020e90a8;

    self->mode = 0;
    if (IsFlag0x40Set(&data_02114e30) && IsFlag0x20Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xc;
    } else if (IsFlag0x40Set(&data_02114e30) && IsFlag0x10Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xd;
    } else if (IsFlag0x80Set(&data_02114e30) && IsFlag0x20Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xe;
    } else if (IsFlag0x80Set(&data_02114e30) && IsFlag0x10Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xf;
    } else if (IsFlag0x40Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 8;
    } else if (IsFlag0x80Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 9;
    } else if (IsFlag0x20Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xa;
    } else if (IsFlag0x10Set(&data_02114e30)) {
        self->mode = 1;
        self->dir = 0xb;
    }

    if (data_02114e54.count >= 5) {
        self->mode = 2;
        struct Vec3 b;
        struct Vec3 a;
        int x1 = f->x1;
        int x0 = f->x0;
        int y1 = f->y1;
        int y0 = f->y0;
        int ay = y1 << 12;
        a.x = x1 << 12;
        a.y = ay;
        a.z = 0;
        b.x = x0 << 12;
        b.y = y0 << 12;
        b.z = 0;
        Vector3fix_Subtract((Vector3fix*)&a, (Vector3fix*)&b, (Vector3fix*)&a);
        if (Vector3fix_Length((Vector3fix*)&a) < 0x8000) {
            self->mode = 0;
        }
    }

    if (self->f154 == 0 || IsField0Null(list) == 0) {
        self->mode = 0;
    }

    if (self->mode == 0) {
        if (self->dir >= 8) {
            self->dir -= 8;
        }
        if (self->fbe != 0) {
            SetByte0xbeShiftPrev((struct Bytes02033b88*)self, 0);
        }
        self->lockedDir = -1;
    } else if (self->mode == 1) {
        if (self->fc0 == 0 || self->fc0 == 0xa) {
            SetByteIfChanged02033b68((struct Obj02033b68*)self, 1);
        } else {
            SetByteSavingPrevious((struct Foo02033b58*)self, 1);
        }
        int base = table.v[self->dir - 8];
        int angle = fix32ReduceAngle0To2Pi(base + GetField0x70(camera));
        if (self->lockedDir == self->dir) {
            angle = self->lockedAngle;
        } else {
            struct Vec3 pos = self->pos;
            struct Box0201e8fc* box = FindBoxContainingPoint0201e8fc((struct Container0201e8fc*)(world + 0x6c), &pos);
            if (box) {
                if (fix32abs(fix32SignedAngleDistance(angle, box->angle)) < 0xc91) {
                    self->lockedDir = self->dir;
                    self->lockedAngle = box->angle;
                } else {
                    short flipped = fix32ReduceAngle0To2Pi(box->angle + 0x3244);
                    if (fix32abs(fix32SignedAngleDistance(angle, flipped)) < 0xc91) {
                        self->lockedDir = self->dir;
                        self->lockedAngle = flipped;
                        angle = self->lockedAngle;
                    }
                }
            } else {
                self->lockedDir = -1;
            }
        }
        SetVecYByMode02033834((struct Obj02033834*)self, angle);
    } else if (self->mode == 2) {
        if (self->fc0 == 0) {
            SetByteIfChanged02033b68((struct Obj02033b68*)self, 1);
        } else {
            SetByteSavingPrevious((struct Foo02033b58*)self, 1);
        }
        struct Field4328* g = GetField4328_0218d268(ov);
        if (g->active != 0) {
            SetVecYByMode02033834((struct Obj02033834*)self, GetShortAt0x22e((struct ShortField0x22e_020968c4*)g));
        }
        self->lockedDir = -1;
    }
}
