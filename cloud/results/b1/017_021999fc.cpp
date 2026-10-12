#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Memory/SafeAllocator.h"
#include "System/ColorEffects.h"
#include "System/Graphics.h"

struct Struct020dfc40;
void ResetStruct020dfc40(Struct020dfc40* texts);
extern "C" void func_020dfec0(void* texts, SafeAllocator* allocator, void* file, unsigned int size);
void SetWord0x18ClearByte0x1f(unsigned char* graphics, int value);
extern "C" void func_0204b5b4(void* graphics, int priority);
struct Obj0204b5e8;
extern "C" void _Z24DispatchViaTable0204b5e8P11Obj0204b5e8ii(Obj0204b5e8* graphics, int a, int b);
int SetBg1ControlBits_02199964(int size, int colorMode, int mapBase, int tileBase, int wrap);
int SetBg2CntFull_021999b4(int screenSize, int colorMode, int screenBase, int charBase);
struct ActiveEntry02046900;
int CountActiveEntries(ActiveEntry02046900* pac);
struct Rec020467f0;
void* FindRecordByIndex(Rec020467f0* pac, int index, void** outName, int* outSize);
extern "C" void func_0204b174(void* graphics, void* file, SafeAllocator* allocator, unsigned int size);
struct Entry_02199684;
void FillEntriesEncoded_02199684(Entry_02199684* arr, int count, int value);
void ClearAndEncodeEntries_021996bc(Entry_02199684* arr, int count);
extern "C" void func_0204bc74(void* graphics, int a, int b, int c, int d, int e, int f);
struct List0204b0e8;
extern "C" void _Z28FlushAndDispatchList0204b0e8P12List0204b0e8Pv(List0204b0e8* graphics, void* characters);
struct Obj0204c7a8;
extern "C" void _Z22AllocateBuffer0204c7a8P11Obj0204c7a8P13SafeAllocatorij(Obj0204c7a8* canvas, SafeAllocator* allocator, int pixels, unsigned int size);
struct Struct_0205cf78;
struct Elem_0205cf78;
extern "C" void _Z29LinkArrayPrevPointers0205cf78P15Struct_0205cf78P13Elem_0205cf78h(Struct_0205cf78* window, Elem_0205cf78* canvases, unsigned char count);

struct FieldMenu021999fc;
extern "C" void func_ov017_0219a388(FieldMenu021999fc* self);

struct BackgroundBytes021999fc {
    unsigned char values[2];
};

extern char data_ov017_021d73f8[];
extern char data_ov017_021d7412[];
extern char data_ov017_021d7423[];
extern const BackgroundBytes021999fc data_ov017_021d643c[2];

struct BackgroundGraphics021999fc {
    char unk_0[0x1c];
    unsigned char unk_1c_0_ : 4;
    unsigned char unk_1c_4_ : 4;
    unsigned char unk_1d;
    char unk_1e[2];
};

struct Canvas021999fc {
    void* unk_0;
    BackgroundGraphics021999fc* background_;
    char unk_8[0xe0 - 8];
};

struct TextWindow021999fc {
    char unk_0[0x98];
    BackgroundGraphics021999fc* background_;
    char unk_9c[0xb2 - 0x9c];
    unsigned char unk_b2;
};

struct FieldMenu021999fc {
    char unk_0[0x178];
    SafeAllocator allocator_;
    char unk_18c[0x3cb0 - 0x18c];
    TextWindow021999fc* window_;
    BackgroundGraphics021999fc backgrounds_[2];
    Canvas021999fc canvases_[4];
    void* canvasPixels_;
    int step_;
    int task_;
    int active_;
    Struct020dfc40* texts_;
};

// USA: func_ov017_021999fc
extern "C" ARM void func_ov017_021999fc(FieldMenu021999fc* self) {
    if (self->canvasPixels_ != 0)
        return;
    if (self->active_ == 0)
        return;
    if (self->step_ == 0)
    {
        self->allocator_.Reset();
        self->texts_ = (Struct020dfc40*)self->allocator_.Allocate(0x18);
        ResetStruct020dfc40(self->texts_);
        self->task_ = BackgroundLoader::GetInstance()->QueueLoadFileInGP2(data_ov017_021d73f8, data_ov017_021d7412, NULL);
        self->step_++;
    }
    else if (self->step_ == 1)
    {
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        if (loader->GetTaskStatus(self->task_))
        {
            void* file;
            unsigned int size;
            loader->GetLoadedFileByID(self->task_, &file, &size);
            func_020dfec0(self->texts_, &self->allocator_, file, size);
            loader->RemoveTask(self->task_);
            self->task_ = -1;
            self->step_++;
        }
    }
    if (self->step_ == 2)
    {
        BackgroundBytes021999fc nibbles = data_ov017_021d643c[0];
        BackgroundBytes021999fc priorities = data_ov017_021d643c[1];
        BackgroundGraphics021999fc* background;
        for (int i = 0; i < 2; i++)
        {
            background = &self->backgrounds_[i];
            SetWord0x18ClearByte0x1f((unsigned char*)background, 0);
            background->unk_1c_0_ = 0;
            background->unk_1c_4_ = nibbles.values[i];
            func_0204b5b4(background, priorities.values[i]);
            _Z24DispatchViaTable0204b5e8P11Obj0204b5e8ii((Obj0204b5e8*)background, 0, 0);
        }
        DISPCNT = (DISPCNT & ~0x1f00) | 0x100;
        SetBg1ControlBits_02199964(0, 0, 0x1d, 0, 0);
        SetBg2CntFull_021999b4(0, 0, 0x1e, 0);
        ColorEffect_ConfigureAlphaBlend(0x04000050, 2, 1, 0xa, 6);
        self->task_ = BackgroundLoader::GetInstance()->QueueLoadFile(data_ov017_021d7423, NULL);
        self->step_++;
    }
    else if (self->step_ == 3)
    {
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        if (loader->GetTaskStatus(self->task_))
        {
            void* name;
            void* file;
            void* files[2];
            unsigned int size;
            unsigned int sizes[2];
            loader->GetLoadedFileByID(self->task_, &file, &size);
            int numFiles = CountActiveEntries((ActiveEntry02046900*)file);
            for (int i = 0; i < numFiles; i++)
            {
                files[i] = FindRecordByIndex((Rec020467f0*)file, i, &name, (int*)&sizes[i]);
            }
            for (int i = 0; i < numFiles; i++)
            {
                if (files[i] != NULL)
                    func_0204b174(&self->backgrounds_[1], files[i], &self->allocator_, sizes[i]);
            }
            loader->RemoveTask(self->task_);
            self->task_ = -1;
            FillEntriesEncoded_02199684((Entry_02199684*)self->backgrounds_, 2, 0x800);
            BackgroundGraphics021999fc* background;
            for (int i = 0; i < 2; i++)
            {
                background = &self->backgrounds_[i];
                func_0204bc74(background, 0, 0, 0, 0x20, 0x19, 0);
                _Z28FlushAndDispatchList0204b0e8P12List0204b0e8Pv((List0204b0e8*)background, NULL);
            }
            ClearAndEncodeEntries_021996bc((Entry_02199684*)self->backgrounds_, 2);
            self->canvasPixels_ = self->allocator_.Allocate(0x400);
            Canvas021999fc* canvas;
            for (int i = 0; i < 4; i++)
            {
                canvas = &self->canvases_[i];
                _Z22AllocateBuffer0204c7a8P11Obj0204c7a8P13SafeAllocatorij((Obj0204c7a8*)canvas, &self->allocator_, (int)self->canvasPixels_, 0x40);
                canvas->background_ = &self->backgrounds_[1];
            }
            TextWindow021999fc* window = self->window_;
            window->background_ = &self->backgrounds_[0];
            window->unk_b2 = 2;
            _Z29LinkArrayPrevPointers0205cf78P15Struct_0205cf78P13Elem_0205cf78h((Struct_0205cf78*)self->window_, (Elem_0205cf78*)self->canvases_, 4);
            BG0CNT = (BG0CNT & ~3) | 2;
            BG1CNT = (BG1CNT & ~3) | 1;
            BG2CNT = BG2CNT & ~3;
            BG3CNT = (BG3CNT & ~3) | 3;
            DISPCNT = (DISPCNT & ~0x1f00) | 0x1700;
            func_ov017_0219a388(self);
            self->step_++;
        }
    }
}
