#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "GameState/GameState.h"
#include "Memory/SafeAllocator.h"
#include "System/ColorEffects.h"
#include "System/Graphics.h"

struct Background0217cf10 {
    char unk_0[0x1c];
    unsigned char unk_1c_0_ : 4;
    unsigned char unk_1c_4_ : 4;
    unsigned char unk_1d;
    char unk_1e[2];
};

struct Canvas0217cf10 {
    void *unk_0;
    Background0217cf10 *background_;
    char unk_8[0xc4 - 8];
    unsigned char unk_c4;
    char unk_c5[0xe0 - 0xc5];
};

struct Window0217cf10 {
    char unk_0[0x98];
    Background0217cf10 *background_;
    char unk_9c[0xb2 - 0x9c];
    unsigned char unk_b2;
    char unk_b3[0xbc - 0xb3];
};

struct Global0217cf10 {
    char unk_0[0x998];
    int unk_998;
    char unk_99c[0x19b2 - 0x99c];
    unsigned char unk_19b2;
};

struct Progress0217cf10 {
    unsigned char pad0[0xb48];
    unsigned int flags : 32;
};

static inline struct Progress0217cf10 *GetProgress(void *run) {
    return (struct Progress0217cf10 *) ((int) run + 0x1840);
}

struct Menu0217cf10 {
    char unk_0[4];
    unsigned char state_;
    signed char step_;
    char unk_6[6];
    int task_;
    int unk_10;
    char unk_14[4];
    unsigned char ids_[0x30 - 0x18];
    unsigned char count_;
    char unk_31;
    unsigned char pages_;
    char unk_33;
    void *unk_34;
    void *unk_38;
    char renderer_[0x78 - 0x3c];
    void *unk_78;
    void *unk_7c;
    char unk_80[8];
    short unk_88;
    char unk_8a[2];
    unsigned char unk_8c;
    char unk_8d[3];
    Window0217cf10 *window_;
    Background0217cf10 *backgrounds_;
    Canvas0217cf10 *canvases_;
    void *pixels_;
    SafeAllocator *allocator_;
    SafeAllocator allocators_[2];
    char unk_cc[1];
};

struct Obj020397cc;
struct Obj0205eaa0;
struct AllocTarget0204b12c;
struct Obj0204b5e8;
struct ActiveEntry02046900;
struct Rec020467f0;
struct List0204b0e8;
struct Obj0204c7a8;
struct Struct_0205cf78;
struct Elem_0205cf78;
struct Container020e0310;

void CancelPendingAction020397cc(Obj020397cc *obj, int a);
void DispatchWithShortB4_0205eaa0(Obj0205eaa0 *obj, int a, int b);
const char *CallFunc020e0434With02153694(int id);
void SetWord0x18ClearByte0x1f(unsigned char *background, int a);
int CountActiveEntries(ActiveEntry02046900 *archive);
void *FindRecordByIndex(Rec020467f0 *archive, int index, void **name, int *size);
void *GetFieldByKey020e0434(Container020e0310 *c, int key);
extern "C" char *func_02012fe4(void);
extern "C" void func_0204500c(void *p, const char *text, int a, int b);
extern "C" void func_0205a528(void *renderer, void *file, unsigned int size, SafeAllocator *allocator);
extern "C" void func_020dfec0(void *texts, SafeAllocator *allocator, void *file, unsigned int size);
extern "C" int func_020420e8(void *obj, int a);
extern "C" void func_0204b5b4(Background0217cf10 *background, int a);
extern "C" void _Z30AllocateAndClearBuffer0204b12cP19AllocTarget0204b12cP13SafeAllocator(AllocTarget0204b12c *background,
                                                                                         SafeAllocator *allocator);
extern "C" void _Z24DispatchViaTable0204b5e8P11Obj0204b5e8ii(Obj0204b5e8 *background, int a, int b);
extern "C" void func_0204b174(Background0217cf10 *background, void *file, SafeAllocator *allocator, unsigned int size);
extern "C" void func_0204bc74(Background0217cf10 *background, int a, int b, int c, int d, int e, int f);
extern "C" void _Z28FlushAndDispatchList0204b0e8P12List0204b0e8Pv(List0204b0e8 *background, void *a);
extern "C" void _Z22AllocateBuffer0204c7a8P11Obj0204c7a8P13SafeAllocatorij(Obj0204c7a8 *canvas, SafeAllocator *allocator,
                                                                           int pixels, unsigned int size);
extern "C" void _Z29LinkArrayPrevPointers0205cf78P15Struct_0205cf78P13Elem_0205cf78h(Struct_0205cf78 *window,
                                                                                     Elem_0205cf78 *canvases,
                                                                                     unsigned char count);
extern "C" void *_Z26GetGlobalField0x1c020421a0v();

extern Obj0205eaa0 data_02108760;
extern char data_ov003_02180c2c[];
extern char data_ov003_02180c43[];
extern char data_ov003_02180c56[];
extern char data_ov003_02180c6f[];

struct BgCnt0217cf10 {
    volatile unsigned short priority_ : 2;
    volatile unsigned short rest_ : 14;
};

// USA: func_ov003_0217cf10
extern "C" ARM void func_ov003_0217cf10(Menu0217cf10 *self) {
    char *current;
    Global0217cf10 *global   = (Global0217cf10 *) _Z26GetGlobalField0x1c020421a0v();
    GameState *state         = GameState::GetInstance();
    BackgroundLoader *loader = BackgroundLoader::GetInstance();
    current                  = func_02012fe4();

    if (self->step_ == 0) {
        CancelPendingAction020397cc((Obj020397cc *) state->GetUnknownGameObject(), 1);
        global->unk_998 = 1;
        DispatchWithShortB4_0205eaa0(&data_02108760, 1, 0);
        if (GetProgress(current)->flags != 0) {
            func_0204500c(global, CallFunc020e0434With02153694(0x42), 0, 0xe3);
            global->unk_19b2 = 0;
            global->unk_998  = 1;
        } else {
            func_0204500c(global, CallFunc020e0434With02153694(0x43), 0, 0xe3);
            global->unk_19b2 = 0;
            global->unk_998  = 1;
            self->state_     = 1;
            self->step_      = 0;
            return;
        }
        int count = 1;
        for (int i = 1; i < 24; i++) {
            if ((1 << i) & ((Progress0217cf10 *) ((int) current + 0x1840))->flags) {
                self->ids_[count] = i;
                count++;
            }
        }
        self->count_ = count;
        self->pages_ = (self->count_ - 1) / 6 + 1;
        self->unk_8c = 0;
        self->unk_7c = self->unk_34;
        self->unk_88 = 7;
        self->unk_78 = self->unk_38;
        self->task_  = loader->QueueLoadFile(data_ov003_02180c2c, 0);
        self->step_  = 1;
    } else if (self->step_ == 1) {
        if (loader->GetTaskStatus(self->task_) != 0) {
            void *name;
            void *file;
            unsigned int size;
            int fileSize;
            loader->GetLoadedFileByID(self->task_, &file, &size);
            int count = CountActiveEntries((ActiveEntry02046900 *) file);
            for (int i = 0; i < count; i++) {
                void *data = FindRecordByIndex((Rec020467f0 *) file, i, &name, &fileSize);
                func_0205a528(self->renderer_, data, fileSize, &self->allocators_[1]);
            }
            loader->RemoveTask(self->task_);
            self->task_ = -1;
            self->task_ = loader->QueueLoadFile(data_ov003_02180c43, 0);
            self->step_ = 2;
        }
    } else if (self->step_ == 2) {
        if (loader->GetTaskStatus(self->task_) != 0) {
            SetWord0x18ClearByte0x1f((unsigned char *) self->backgrounds_, 0);
            self->backgrounds_->unk_1c_0_ = 0;
            self->backgrounds_->unk_1c_4_ = 1;
            func_0204b5b4(self->backgrounds_, 0);
            _Z30AllocateAndClearBuffer0204b12cP19AllocTarget0204b12cP13SafeAllocator((AllocTarget0204b12c *) self->backgrounds_,
                                                                                     self->allocator_);
            _Z24DispatchViaTable0204b5e8P11Obj0204b5e8ii((Obj0204b5e8 *) self->backgrounds_, 0, 0);
            BG1CNT = (BG1CNT & 0x43) | 0xd08;
            SetWord0x18ClearByte0x1f((unsigned char *) &self->backgrounds_[1], 0);
            self->backgrounds_[1].unk_1c_0_ = 0;
            self->backgrounds_[1].unk_1c_4_ = 2;
            func_0204b5b4(&self->backgrounds_[1], 1);
            _Z30AllocateAndClearBuffer0204b12cP19AllocTarget0204b12cP13SafeAllocator((AllocTarget0204b12c *) &self->backgrounds_[1],
                                                                                     self->allocator_);
            _Z24DispatchViaTable0204b5e8P11Obj0204b5e8ii((Obj0204b5e8 *) &self->backgrounds_[1], 0, 0);
            BG2CNT = (BG2CNT & 0x43) | 0xe08;
            void *name;
            void *file;
            void *datas[2];
            unsigned int size;
            int sizes[2];
            loader->GetLoadedFileByID(self->task_, &file, &size);
            int count = CountActiveEntries((ActiveEntry02046900 *) file);
            for (int i = 0; i < count; i++) {
                datas[i] = FindRecordByIndex((Rec020467f0 *) file, i, &name, &sizes[i]);
            }
            for (int i = 0; i < count; i++) {
                if (datas[i] != 0) func_0204b174(self->backgrounds_, datas[i], self->allocator_, sizes[i]);
            }
            loader->RemoveTask(self->task_);
            self->task_ = -1;
            func_0204bc74(self->backgrounds_, 0, 0, 0, 0x20, 0x19, 0);
            _Z28FlushAndDispatchList0204b0e8P12List0204b0e8Pv((List0204b0e8 *) self->backgrounds_, 0);
            func_0204bc74(&self->backgrounds_[1], 0, 0, 0, 0x20, 0x19, 0);
            _Z28FlushAndDispatchList0204b0e8P12List0204b0e8Pv((List0204b0e8 *) &self->backgrounds_[1], 0);
            self->pixels_          = self->allocator_->Allocate(0x2800);
            _Z22AllocateBuffer0204c7a8P11Obj0204c7a8P13SafeAllocatorij((Obj0204c7a8 *) self->canvases_, self->allocator_,
                                                                       (int) self->pixels_, 0x800);
            self->canvases_->background_ = self->backgrounds_;
            self->canvases_->unk_c4      = 0;
            Window0217cf10 *window = self->window_;
            window->background_    = self->backgrounds_;
            window->unk_b2         = 2;
            _Z29LinkArrayPrevPointers0205cf78P15Struct_0205cf78P13Elem_0205cf78h((Struct_0205cf78 *) self->window_,
                                                                                 (Elem_0205cf78 *) self->canvases_, 1);
            ((BgCnt0217cf10 *) 0x4000008)[0].priority_ = 3;
            ((BgCnt0217cf10 *) 0x4000008)[1].priority_ = 1;
            ((BgCnt0217cf10 *) 0x4000008)[2].priority_ = 2;
            ((BgCnt0217cf10 *) 0x4000008)[3].priority_ = 0;
            ColorEffect_ConfigureAlphaBlend(0x4000050, 4, 1, 0xa, 6);
            self->task_ = loader->QueueLoadFileInGP2(data_ov003_02180c56, data_ov003_02180c6f, 0);
            self->step_ = 3;
        }
    } else if (self->step_ == 3) {
        if (loader->GetTaskStatus(self->task_) != 0) {
            void *file;
            unsigned int size;
            loader->GetLoadedFileByID(self->task_, &file, &size);
            self->allocators_[0].Reset();
            func_020dfec0(self->unk_cc, &self->allocators_[0], file, size);
            loader->RemoveTask(self->task_);
            self->task_ = -1;
            int max = 0;
            for (int key = 100; key < 0x7c; key++) {
                int v = 0;
                void *obj = GetFieldByKey020e0434((Container020e0310 *) self->unk_cc, (short) key);
                if (obj != 0) v = func_020420e8(obj, 0);
                if (max < v) max = v;
            }
            int aligned = max + 0x1b;
            aligned &= ~7;
            int cells = aligned >> 3;
            cells     = (cells + 1) & ~1;
            self->unk_10 = cells << 3;
            DISPCNT = (DISPCNT & ~0x1f00) | 0x1700;
            self->state_ = 2;
            self->step_  = 0;
        }
    }
}
