#include <globaldefs.h>
#include <GameState/GameState.h>

struct TouchState
{
    char unk_0[0x24];
    unsigned short unk_24;
    char unk_26[0x54 - 0x26];
    unsigned char unk_54;
    unsigned char touching_;
    char unk_56[0x5f - 0x56];
    unsigned char unk_5f;
};

struct MemberScreen
{
    char unk_0[0x4fc];
    int member_;
    char unk_500[0x634 - 0x500];
    unsigned short flags_;
};

struct MenuModel
{
    char unk_0[0x88];
};

struct CursorFrame
{
    MenuModel* model_;
    int depth_;
    int left_;
    int right_;
    int top_;
    int bottom_;
    char unk_18[0x38 - 0x18];
    int scale_;
};

struct EquipmentMenu
{
    char unk_0[0x19f4];
    char cursor_[0x1a34 - 0x19f4];
    CursorFrame frame_;
    MenuModel models_[36];
    char unk_2d90[0x3daa - 0x2d90];
    unsigned char pageStep_;
    char unk_3dab[0x3db8 - 0x3dab];
    unsigned char state_;
    unsigned char lastState_;
    unsigned char step_;
    signed char slot_;
    unsigned char kind_;
    signed char page_;
    char unk_3dbe[0x3dcc - 0x3dbe];
    unsigned int flags_;
    char unk_3dd0[0x3de0 - 0x3dd0];
    short swapped_;
};

struct PartyData
{
    char unk_0[0x1d4];
    unsigned char bag_[1];
};

struct Obj0205eaa0;
struct Struct_0205bb84;

extern "C" MemberScreen* _Z19GetField1c_021a193cPi(int* obj);
extern "C" void _Z28DispatchByIdxAndCond021551fcPchi(EquipmentMenu* self, int member, int change);
extern "C" void _Z22SelectCoordsByFlag0x24PhPiS0_(TouchState* touch, int* x, int* y);
extern "C" void func_ov005_02156658(EquipmentMenu* self, int x, int y);
extern "C" int func_ov005_02155670(EquipmentMenu* self, int x, int y);
extern "C" void func_0205bb04(void* cursor, int n);
extern "C" void func_ov005_021579ec(EquipmentMenu* self, unsigned int state, unsigned char slot);
extern "C" int func_0205bf58(void* cursor, int ticks);
extern "C" int _Z25TestFlag0SetAndFlag1ClearPti(unsigned short* obj, int mask);
extern "C" int func_ov005_021585fc(EquipmentMenu* self, int direction);
extern "C" void func_ov005_021555c0(EquipmentMenu* self);
extern "C" void func_ov005_021536e0(CursorFrame* frame);
extern "C" void func_ov005_021553b4(EquipmentMenu* self, int slot, int* x, int* y);
extern "C" void func_ov005_02154d18(CursorFrame* frame, int x, int y);
extern "C" void func_ov005_02154d34(CursorFrame* frame, int x, int y);
extern "C" void func_ov005_02154d50(CursorFrame* frame, int width, int height);
extern "C" void func_ov005_02154d7c(CursorFrame* frame, int width, int height);
extern "C" void func_ov005_021537bc(CursorFrame* frame, int mode, unsigned short color);
extern "C" int _Z24ComputeScaledSum0205bb84P15Struct_0205bb84(Struct_0205bb84* s);
extern "C" void _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(Obj0205eaa0* obj, int a, int b);
extern "C" int _Z25CheckFlag30Or401_02157190v(EquipmentMenu* self);
extern "C" PartyData* _Z17GetPtrField0x2a04P9GameState(GameState* gameState);
extern "C" short* _Z24GetPointerFromArray0xbd0Phj(unsigned char* p, unsigned int list);
extern "C" signed char* _Z17GetPointerAt0xbf0Pvj(void* p, unsigned int list);
extern "C" void func_ov005_0215792c(EquipmentMenu* self, unsigned char state);

extern "C" TouchState data_02114e54;
extern "C" unsigned short data_02114e30;
extern "C" Obj0205eaa0 data_02108760;
extern "C" unsigned char data_ov005_0215cd10[];

// USA: func_ov005_0215c058
extern "C" ARM void func_ov005_0215c058(EquipmentMenu* self)
{
    if (self->step_ == 0)
    {
        self->swapped_ = self->slot_ - 8 + self->page_ * 16;
        self->frame_.model_ = &self->models_[30];
        self->step_++;
    }
    else if (self->step_ == 1)
    {
        MemberScreen* screen = _Z19GetField1c_021a193cPi((int*)func_ov017_0218b5b0()->unknown_ptr_array_36fc[3]);
        _Z28DispatchByIdxAndCond021551fcPchi(self, screen->member_, 0);
        GameState* gameState = GameState::GetInstance();
        int touched = 0;
        int selected = 0;
        if (data_02114e54.touching_)
        {
            int x;
            int y;
            touched = 1;
            _Z22SelectCoordsByFlag0x24PhPiS0_(&data_02114e54, &x, &y);
            self->flags_ = (self->flags_ & ~4) | 8;
            func_ov005_02156658(self, x, y);
            int slot = func_ov005_02155670(self, x, y);
            if (slot >= 8 && slot < 24)
            {
                self->slot_ = slot;
                selected = touched;
                func_0205bb04(self->cursor_, self->slot_ - 8);
                func_ov005_021579ec(self, self->state_, self->slot_);
            }
        }
        else if (data_02114e54.unk_5f != 0 && data_02114e54.unk_24 != 0)
        {
            touched = 1;
        }
        else if (data_02114e54.unk_54 != 0)
        {
            touched = 1;
        }
        if (!touched)
        {
            unsigned int ticks = gameState->GetTickCount();
            if (ticks == 0)
                ticks = 1;
            func_0205bf58(self->cursor_, ticks);
            int moved = 0;
            int column = (self->slot_ - 8) % 4;
            if (column == 0)
            {
                if (_Z25TestFlag0SetAndFlag1ClearPti(&data_02114e30, 0x20))
                    moved = func_ov005_021585fc(self, -1);
            }
            else if (column == 3)
            {
                if (_Z25TestFlag0SetAndFlag1ClearPti(&data_02114e30, 0x10))
                    moved = func_ov005_021585fc(self, 1);
            }
            if (moved)
                func_ov005_021555c0(self);
        }
        short first = self->page_ * 16;
        short last = first + 16;
        if (self->swapped_ >= first && self->swapped_ < last)
        {
            CursorFrame frame;
            int x;
            int y;
            func_ov005_021536e0(&frame);
            frame.model_ = &self->models_[26];
            frame.depth_ = 0x8000;
            frame.left_ = -0x3000;
            frame.right_ = 0x3000;
            frame.bottom_ = 0x3000;
            frame.top_ = -0x3000;
            func_ov005_021553b4(self, self->swapped_ % 16 + 8, &x, &y);
            int fx = x << 12;
            int fy = y << 12;
            func_ov005_02154d18(&frame, fx, fy);
            func_ov005_02154d34(&frame, fx, fy);
            func_ov005_02154d50(&frame, 0x18000, 0x18000);
            func_ov005_02154d7c(&frame, 0x18000, 0x18000);
            frame.scale_ = 0x4800;
            func_ov005_021537bc(&frame, 0, 0x7fff);
        }
        int slot = _Z24ComputeScaledSum0205bb84P15Struct_0205bb84((Struct_0205bb84*)self->cursor_) + 8;
        if (slot != self->slot_)
        {
            self->slot_ = slot;
            _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 2, 0);
            func_ov005_021579ec(self, self->state_, self->slot_);
        }
        else if (slot == self->slot_)
        {
            if (_Z25CheckFlag30Or401_02157190v(self) || selected)
            {
                short index = self->slot_ - 8 + self->page_ * 16;
                PartyData* party = _Z17GetPtrField0x2a04P9GameState(gameState);
                short* items = _Z24GetPointerFromArray0xbd0Phj(party->bag_, data_ov005_0215cd10[self->kind_]);
                signed char* counts = _Z17GetPointerAt0xbf0Pvj(party->bag_, data_ov005_0215cd10[self->kind_]);
                short item = items[self->swapped_];
                signed char count = counts[self->swapped_];
                items[self->swapped_] = items[index];
                counts[self->swapped_] = counts[index];
                items[index] = item;
                counts[index] = count;
                func_ov005_021555c0(self);
                self->flags_ |= 0x40;
                self->pageStep_ = 0;
                self->swapped_ = -1;
                func_ov005_0215792c(self, 4);
                _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 1, 0);
                screen->flags_ |= 0x81;
                self->frame_.model_ = &self->models_[0];
                self->flags_ |= 0xc;
            }
            else if (_Z25TestFlag0SetAndFlag1ClearPti(&data_02114e30, 2))
            {
                self->swapped_ = -1;
                func_ov005_0215792c(self, 4);
                screen->flags_ |= 0x81;
                self->frame_.model_ = &self->models_[0];
                self->flags_ |= 0xc;
            }
        }
    }
}
