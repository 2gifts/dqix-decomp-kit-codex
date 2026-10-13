#include <globaldefs.h>

struct Obj0208203c;
struct Obj0205eaa0;
struct Obj2081;
struct Obj0215951c;
struct Obj02159094;
struct StructC28;
struct Struct021596f0;

struct Recipe {
    short id_;
    short item_;
    char unk_4[0x18];
    Recipe* next_;
};

struct AlchemyPot {
    char unk_0[0x1258];
    unsigned short windowFlags_;
};

struct Menu {
    char unk_0[0x36];
    short cursor_;
};

struct AlchemyMenu {
    int textPosition_;
    char** texts_;
    void* canvasBuffer_;
    void* allocators_;
    AlchemyPot* pot_;
    Menu* menu_;
    void* choice_;
    void* backgrounds_;
    void* canvases_;
    void* sprites_;
    void* animations_;
    Recipe* recipes_;
    Recipe* pageStart_;
    Recipe* recipe_;
    void* records_;
    char save_[8];
    short* cursor_;
    short** items_;
    unsigned char** counts_;
    unsigned short* sizes_;
    int subBackground_[0x20 / 4];
    int ingredients_[0x28 / 4];
    int repeat_[0xc / 4];
    unsigned short buttons_;
    unsigned char unk_aa;
    int renderer_[0x54 / 4];
    int layout_[0x4c / 4];
    int table_[0xc / 4];
    int menuTexts_[0x18 / 4];
    char itemNames_[0xc];
    int results_[4][0x74 / 4];
    int ticks_;
    int menuResult_;
    int task_;
    int unk_358;
    short unk_35c;
    short previousCursor_;
    short mainCursor_;
    short categoryCursor_;
    short itemCursor_;
    short bookCursor_;
    short filterCursor_;
    short recipeCursor_;
    short choiceCursor_;
    short group_;
    short item_;
    unsigned short categories_[3];
    short chosenItems_[3];
    short message_;
    unsigned short messageLength_;
    short successRate_;
    signed char filterCategory_;
    signed char filterKind_;
    unsigned char sort_;
    unsigned char times_;
    unsigned char page_;
    unsigned char pages_;
    unsigned char category_;
    unsigned char count_;
    unsigned char chosenCounts_[3];
    unsigned char state_;
    unsigned char step_;
    unsigned char unk_391;
    unsigned char messageStep_;
    unsigned char repeatDelay_;
    unsigned short flags_;
    unsigned char female_;
    unsigned char background_;
    unsigned char amounts_[3];
    int textSound_;
    int unk_3a0;
    int textSoundOn_;
    unsigned int textSoundTimer_;
    int textSoundPlaying_;
    int textSoundState_;
    int showResult_;
    int resultSprite_[0x70 / 4];
    int resultTask_;
    signed char arrowTimer_;
    unsigned char fadeTimer_;
    unsigned char nextStep_;
    unsigned char saved_;
    unsigned char arrowUp_;
    unsigned char arrowDown_;
    unsigned char resetBlend_;
    unsigned char closing_;
    unsigned char closeRequested_;
};

void CallFunc0204c804OnMatchingKey(Obj2081* menu, int group);
short FindMappedMemberId02080468(void* menu, int group);
void SetElementFlag0x40(Obj2081* menu, int group, int flag);
extern "C" void func_020813ec(void* menu, int group);
void SetEntryLowNibbleAndElement02080c68(void* menu, int group, int value);
void CalcField389_0215951c(Obj0215951c* self);
void ClearSublistEntriesFlag4(void* menu, int group);
extern "C" void func_ov006_0215f4dc(AlchemyMenu* self);
void ClearFlagsForIds_021573c8(void* layout);
extern "C" void func_ov006_021553bc(AlchemyPot* pot, short* items, unsigned char* amounts);
void ResetWithSub0208203c(Obj0208203c* repeat);
extern "C" void func_ov006_02155710(AlchemyPot* pot, int value, int flag);
extern "C" int func_ov006_021595b4(AlchemyMenu* self);
extern "C" int func_ov006_02158a28(AlchemyMenu* self);
void DispatchWithShortB4_0205eaa0(Obj0205eaa0* sound, int, int);
extern "C" int func_ov006_02159218(AlchemyMenu* self);
extern "C" int func_ov006_02158b78(AlchemyMenu* self);
void ExpireStatusSlot02159094(Obj02159094* self);
void ClearField44IfSet_02158c28(StructC28* self);
extern "C" void func_ov006_0215943c(AlchemyMenu* self);
int Check02159564(void* self);
extern "C" void func_ov006_02159274(AlchemyMenu* self, unsigned char* category, short* item, unsigned char* count);
void UpdateEntryFlags021596f0(Struct021596f0* self);
extern "C" int func_ov006_02159834(AlchemyMenu* self);
int TestFlag0SetAndFlag1Clear(unsigned short* flags, int mask);
extern "C" void func_ov006_02159108(struct Struct02159108* self, int category, int item, int count);

extern "C" char data_02108760[];
extern "C" char data_02114e30[];

// USA: func_ov006_0215c154
extern "C" ARM void func_ov006_0215c154(AlchemyMenu* self) {
    Obj2081* menu = (Obj2081*)self->menu_;
    if (self->step_ == 0)
    {
        CallFunc0204c804OnMatchingKey(menu, 0);
        CallFunc0204c804OnMatchingKey(menu, 1);
        self->group_ = 0x10;
        if (self->categoryCursor_ < 0)
            self->categoryCursor_ = FindMappedMemberId02080468(menu, self->group_);
        SetElementFlag0x40(menu, self->group_, 0);
        func_020813ec(menu, self->group_);
        SetEntryLowNibbleAndElement02080c68(menu, self->group_, 0);
        self->item_ = -1;
        self->count_ = 0;
        CalcField389_0215951c((Obj0215951c*)self);
        SetElementFlag0x40(menu, 0x11, 0);
        ClearSublistEntriesFlag4(menu, 0x11);
        func_ov006_0215f4dc(self);
        self->flags_ |= 0x10;
        SetEntryLowNibbleAndElement02080c68(menu, 0x11, 1);
        ClearFlagsForIds_021573c8(self->layout_);
        func_ov006_021553bc(self->pot_, self->chosenItems_, self->chosenCounts_);
        ResetWithSub0208203c((Obj0208203c*)self->repeat_);
        self->cursor_ = 0;
        func_ov006_02155710(self->pot_, -1, 0);
        self->step_++;
    }
    else if (self->step_ == 1)
    {
        if (func_ov006_021595b4(self))
            return;
        self->cursor_ = &self->categoryCursor_;
        if (self->previousCursor_ != *self->cursor_)
        {
            CalcField389_0215951c((Obj0215951c*)self);
            func_ov006_0215f4dc(self);
        }
        if (func_ov006_02158a28(self))
        {
            DispatchWithShortB4_0205eaa0((Obj0205eaa0*)data_02108760, 1, 0);
            ResetWithSub0208203c((Obj0208203c*)self->repeat_);
            self->cursor_ = 0;
            if (!func_ov006_02159218(self))
                return;
            SetEntryLowNibbleAndElement02080c68(menu, self->group_, 1);
            SetElementFlag0x40(menu, self->group_, 1);
            SetEntryLowNibbleAndElement02080c68(menu, 0x11, 0);
            self->step_++;
        }
        else if (func_ov006_02158b78(self))
        {
            ResetWithSub0208203c((Obj0208203c*)self->repeat_);
            self->cursor_ = 0;
            self->state_ = 5;
            self->step_ = 0;
            ExpireStatusSlot02159094((Obj02159094*)self);
            func_ov006_021553bc(self->pot_, self->chosenItems_, self->chosenCounts_);
            func_ov006_0215f4dc(self);
        }
        ClearField44IfSet_02158c28((StructC28*)self);
    }
    else if (self->step_ == 2)
    {
        func_ov006_0215943c(self);
        func_ov006_021553bc(self->pot_, self->chosenItems_, self->chosenCounts_);
        self->step_++;
    }
    else if (self->step_ == 3)
    {
        if (Check02159564(self))
            return;
        if (func_ov006_02158a28(self) && self->previousCursor_ == *self->cursor_)
        {
            unsigned char category = 0;
            unsigned char done = 0;
            short item = 0;
            func_ov006_02159274(self, &category, &item, &done);
            if (done)
            {
                DispatchWithShortB4_0205eaa0((Obj0205eaa0*)data_02108760, 1, 0);
                ResetWithSub0208203c((Obj0208203c*)self->repeat_);
                self->cursor_ = 0;
                SetElementFlag0x40((Obj2081*)self->menu_, 0x11, 1);
                self->category_ = category;
                self->item_ = item;
                self->count_ = 1;
                self->step_++;
                self->flags_ &= ~0x10;
                self->flags_ |= 0x20;
                self->message_ = 9;
                self->messageStep_ = 0;
                return;
            }
        }
        else if (func_ov006_02158b78(self))
        {
            ResetWithSub0208203c((Obj0208203c*)self->repeat_);
            self->cursor_ = 0;
            self->itemCursor_ = -1;
            self->step_ = 0;
        }
        ClearField44IfSet_02158c28((StructC28*)self);
    }
    else if (self->step_ == 4)
    {
        UpdateEntryFlags021596f0((Struct021596f0*)self);
        self->repeatDelay_ = 0;
        self->step_++;
    }
    else if (self->step_ == 5)
    {
        self->arrowTimer_ = 0;
        int result = func_ov006_02159834(self);
        if (result == 1)
            return;
        if (TestFlag0SetAndFlag1Clear((unsigned short*)data_02114e30, 1) || result == 2)
        {
            DispatchWithShortB4_0205eaa0((Obj0205eaa0*)data_02108760, 1, 0);
            ResetWithSub0208203c((Obj0208203c*)self->repeat_);
            self->cursor_ = 0;
            self->flags_ &= ~8;
            CallFunc0204c804OnMatchingKey(menu, 6);
            func_ov006_02159108((struct Struct02159108*)self, self->category_, self->item_, self->count_);
            func_ov006_021553bc(self->pot_, self->chosenItems_, self->chosenCounts_);
            self->state_ = 7;
            self->step_ = 0;
            self->times_ = self->count_;
            self->message_ = 0x3a;
            self->messageStep_ = 0;
            self->flags_ |= 0x120;
        }
        else if (func_ov006_02158b78(self) || result == 3)
        {
            ResetWithSub0208203c((Obj0208203c*)self->repeat_);
            self->cursor_ = 0;
            self->flags_ &= ~8;
            CallFunc0204c804OnMatchingKey(menu, 6);
            self->step_ = 0;
            self->message_ = 0x24;
            self->messageStep_ = 0;
            self->flags_ |= 0x120;
        }
    }
}
