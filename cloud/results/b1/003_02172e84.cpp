#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "GameState/GameState.h"
#include "Resource/GameResources.h"
#include "std_library_functions.h"

struct Container0205a330;

struct Battle02172e84 {
    char pad0[0x28];
    unsigned char active;

    unsigned char IsActive() const {
        unsigned char v = active;
        return v;
    }
};

struct Progress02172e84 {
    char pad0[0xb48];
    unsigned int seenMask;
    unsigned int newMask;
};

struct Messages02172e84 {
    char pad0[0x2d8];
    void *source;
    int sourceA;
    Container0205a330 *container;
    char pad2e4[0x998 - 0x2e4];
    int busy;
    int field99c;
    int state;
    char pad9a4[0x19be - 0x9a4];
    unsigned char shown;
};

struct Member02172e84 {
    char pad0[0x6c];
    unsigned int unk6c_lo : 25;
    unsigned int gender : 1;
    unsigned int unk6c_hi : 6;
    int unk70_lo : 19;
    int species : 11;
    int unk70_hi : 2;
    char pad74[0xe8 - 0x74];

    bool HasPortrait() const {
        return species < 700;
    }
};

struct Party02172e84 {
    unsigned char state;
    char pad1[0x460 - 1];
    Member02172e84 members[3];
    unsigned char count;
    char pad719[0x738 - 0x719];
    unsigned int mask;

    int IsBusy() const {
        return state == 8 || state == 9;
    }
};

struct Table02172e84 {
    void *entries;
    int count;
};

struct Touch02172e84 {
    char pad0[0x30];
    int x;
    int y;
    char pad38[0x54 - 0x38];
    unsigned char released;
};

struct Self02172e84 {
    unsigned char touched;
    unsigned char changed;
    short id;
    char title[0x40];
    char text[0x80];
    unsigned char shownCount;
    char padc5[0xca - 0xc5];
    unsigned char confirm;
    unsigned char dirty;
    char padcc[0xd0 - 0xcc];
    Party02172e84 *party;
    unsigned char shown;
    unsigned char next;
    char padd6[0xd8 - 0xd6];
    char workd8[0xec - 0xd8];
    unsigned char mode;
    unsigned char subStep;
    unsigned char step;
    char padef[0x128 - 0xef];
    int source128;
    Container0205a330 *container12c;
    char block130[0x1c0 - 0x130];
    int arg1c0;
    char pad1c4[0x1d0 - 0x1c4];
    Table02172e84 names;
    Table02172e84 messages;
    char allocators[0x30d - 0x1e0];
    unsigned char battleByte;
    char pad30e[0x340 - 0x30e];
    void *work;
    unsigned char lidOpen;
};

extern "C" Messages02172e84 *_Z26GetGlobalField0x1c020421a0v();
int IsMainBrightnessTransitionActive(GameResources *resources);
extern "C" const char *_Z14FindEntryByKeyP8TableA68i(Table02172e84 *table, int key);
extern "C" void func_0204500c(Messages02172e84 *msgs, const char *text, int a, int b);
extern "C" void *func_02012fe4(void);
extern "C" void __clear(void *dst, int size);
extern "C" void func_02042764(Member02172e84 *member, char *name, int flag);
extern "C" void *ExtractFileFromGP2(const char *gp2Path, const char *innerFilePath, unsigned int *outSize);
extern "C" const char *func_020e046c(char *output, void *file, unsigned int size, short id);
extern "C" void func_ov003_0217bd38(void *allocators, int arg);
unsigned char GetByteField28_021b1fa8(void *obj);
extern "C" void *func_0202ae18(void);
int CheckField0NonZero(int *p);
extern "C" void func_0202b0f4(void *p);
void SelectCoordsByFlag0x24(unsigned char *obj, int *out1, int *out2);
int IsInRangePair_021734dc(int unused, int a, int b);
int TestFlag0SetAndFlag1Clear(unsigned short *obj, int mask);
void IterateEntries0205a330(Container0205a330 *c, int arg);
extern "C" int func_020457e0(Messages02172e84 *msgs);
extern "C" int func_ov003_02172570(Party02172e84 *party, void *extra);

extern const char data_ov003_02180a49[];
extern const char data_ov003_02180a4c[];
extern const char data_ov003_02180a63[];
extern Touch02172e84 data_02114e54;
extern unsigned short data_02114e30;

static inline Progress02172e84 *GetProgress(void *zone) {
    return (Progress02172e84 *)((int)zone + 0x1840);
}

// USA: func_ov003_02172e84
extern "C" ARM void func_ov003_02172e84(Self02172e84 *self) {
    GameResources *res     = func_ov017_0218b5b0();
    Messages02172e84 *msgs = _Z26GetGlobalField0x1c020421a0v();
    Battle02172e84 *battle = (Battle02172e84 *)res->unknown_ptr_array_3afc[19];
    unsigned char step     = self->step;
    if (step == 0) {
        if (IsMainBrightnessTransitionActive(res) == 0) {
            func_0204500c(msgs, _Z14FindEntryByKeyP8TableA68i(&self->messages, 100), 0, 0xe3);
            msgs->shown = 1;
            msgs->busy  = 1;
            self->step  = 1;
        }
    } else if (step == 1) {
        unsigned char count;
        unsigned char active;
        void *zone = func_02012fe4();
        unsigned int mask = self->party->mask & ~1;
        active = battle->IsActive();
        mask &= mask ^ GetProgress(zone)->seenMask;
        count = self->party->count;
        if (active != 0) {
            if (!self->party->IsBusy()) {
                self->lidOpen = 1;
            }
        }
        if (mask != 0 && self->shown < 3) {
            self->shown++;
            self->dirty   = 1;
            self->changed = 1;
            int i;
            for (i = 1; i < 24; i++) {
                if (mask & (1 << i)) break;
            }
            Progress02172e84 *progress = (Progress02172e84 *)((char *)zone + 0x1840);
            progress->seenMask |= 1 << i;
            progress->newMask |= 1 << i;
            sprintf(self->title, data_ov003_02180a49, _Z14FindEntryByKeyP8TableA68i(&self->messages, (short)i));
            self->id = i;
            self->shownCount++;
            self->step = 0x63;
        } else if (self->next != count && self->shown < 3) {
            Member02172e84 *member = &self->party->members[self->next];
            self->next++;
            self->dirty = 1;
            self->shown++;
            char name[0x30];
            __clear(name, 0x30);
            func_02042764(member, name, 1);
            sprintf(self->title, data_ov003_02180a49, name);
            if (member->HasPortrait()) {
                BackgroundLoader::AddLockGlobal();
                unsigned int size = 0;
                char path[0x40];
                char inner[0x20];
                __clear(path, 0x40);
                __clear(inner, 0x20);
                sprintf(path, data_ov003_02180a4c, member->gender);
                sprintf(inner, data_ov003_02180a63, member->gender);
                void *file = ExtractFileFromGP2(path, inner, &size);
                if (file != 0) {
                    func_020e046c(self->text, file, size, (short)member->species);
                }
                BackgroundLoader::RemoveLockGlobal();
            } else {
                sprintf(self->text, data_ov003_02180a49,
                        _Z14FindEntryByKeyP8TableA68i(&self->names, (short)(member->species + 20000)));
            }
            self->step = 0x63;
        } else if (self->lidOpen != 0 && !((*(unsigned short *)0x27fffa8 & 0x8000) >> 15)) {
            self->work = self->workd8;
            func_ov003_0217bd38(self->allocators, self->arg1c0);
            self->battleByte   = GetByteField28_021b1fa8(battle);
            self->party->state = 10;
            void *search       = func_0202ae18();
            if (CheckField0NonZero((int *)search)) {
                func_0202b0f4(search);
            }
            self->mode = 5;
            self->step = 0;
        } else {
            int x = data_02114e54.x;
            int y = data_02114e54.y;
            int tx;
            int ty;
            SelectCoordsByFlag0x24((unsigned char *)&data_02114e54, &tx, &ty);
            if (IsInRangePair_021734dc((int)self, x, y) && IsInRangePair_021734dc((int)self, tx, ty)) {
                self->touched = 1;
                if (data_02114e54.released != 0) {
                    self->step = 2;
                }
            } else if (TestFlag0SetAndFlag1Clear(&data_02114e30, 2)) {
                self->touched = 1;
                self->step    = 2;
            } else {
                self->touched = 0;
            }
        }
    } else if (step == 2) {
        func_0204500c(msgs, _Z14FindEntryByKeyP8TableA68i(&self->messages, 0x65), 0, 0xe3);
        msgs->busy = 1;
        self->step = 3;
    } else if (step == 3) {
        GameState *gs   = GameState::GetInstance();
        msgs->source    = self->block130;
        msgs->sourceA   = self->source128;
        msgs->container = self->container12c;
        self->touched   = 0;
        Container0205a330 *container = msgs->container;
        IterateEntries0205a330(container, gs->GetTickCount() >> 2);
        if (msgs->state == 0) {
            msgs->busy = 1;
            if (func_020457e0(msgs) == 0) {
                self->confirm = 1;
                self->step    = 4;
            } else {
                self->step = 0;
            }
        }
    } else if (step == 4) {
        if (self->confirm != 0 && !self->party->IsBusy()) {
            self->mode         = 4;
            self->step         = 0;
            self->party->state = 10;
        } else if (self->party->IsBusy()) {
            func_ov003_02172570(self->party, 0);
        }
    } else if (step == 0x63) {
        if (self->subStep == 2) {
            self->step = 1;
        }
    }
}
