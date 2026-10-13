#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Vector.h"

struct TransferState {
    char pad0[0xa]; unsigned char result_; char padB;
    signed char player_; char padD[0x25 - 0xd]; unsigned char phase_;
    char pad26[0x288 - 0x26]; void* data_;
};
struct TransferSettings { char pad0[0x2784]; short field2784_; short field2786_; unsigned char enabled_; };
struct PlayerLocation { unsigned short zone_; Vector3fix16 position_; };
extern "C" TransferSettings* func_02012fe4();
void* GetData02100044();
void EnqueueEventTag147_021cdaa0();
extern "C" void func_ov017_021cf730(int, int);
void EnqueueEventTag162_021cfc74(unsigned char);
extern "C" void func_ov017_021d2184(unsigned char);
PlayerLocation* GetField0x74deForValidIndex(char*, unsigned int);
extern "C" void func_ov017_021d360c(int, unsigned char, unsigned short, Vector3fix16);
extern "C" void func_ov017_021d1a18(int, int, int, int);
int CheckSlotsAllFree0205e488(void*);
extern "C" void func_ov017_021cdc24(unsigned char, unsigned int);
extern "C" void func_ov017_021d22d4(unsigned char, unsigned short, void*);
struct TransferProgress { unsigned int offset_; };
extern TransferProgress data_ov017_021d83d4;

// USA: func_ov017_021b06b0
extern "C" ARM unsigned char func_ov017_021b06b0(TransferState* self) {
    GameState* state = GameState::GetInstance();
    TransferSettings* settings = func_02012fe4();
    GrottoStruct* grotto = state->GetGrottoStruct();
    void* queue = GetData02100044();
    if (self->phase_ == 0) {
        EnqueueEventTag147_021cdaa0();
        func_ov017_021cf730(self->player_, 1);
        EnqueueEventTag162_021cfc74(grotto->unknown_0[3]);
        func_ov017_021d2184(self->player_);
        for (int i = 0; i < 4; ++i) {
            PlayerLocation* location = GetField0x74deForValidIndex((char*)state, i);
            if (location) func_ov017_021d360c(0, i, location->zone_, location->position_);
        }
        if (settings->enabled_) func_ov017_021d1a18(1, -1, 1, 0);
        else func_ov017_021d1a18(settings->field2786_, settings->field2784_, 0, 0);
        self->phase_ = 1;
    } else if (self->phase_ == 1) {
        if (CheckSlotsAllFree0205e488(queue)) {
            data_ov017_021d83d4.offset_ = 0;
            self->phase_ = 2;
        }
    } else if (self->phase_ == 2) {
        for (int i = 0; i < 48; ++i) {
            func_ov017_021cdc24(self->player_, data_ov017_021d83d4.offset_);
            data_ov017_021d83d4.offset_ += 14;
            if (data_ov017_021d83d4.offset_ >= 253) break;
        }
        self->phase_ = 3;
    } else if (self->phase_ == 3) {
        if (CheckSlotsAllFree0205e488(queue)) {
            if (data_ov017_021d83d4.offset_ >= 253) {
                data_ov017_021d83d4.offset_ = 0;
                self->phase_ = 4;
            } else self->phase_ = 2;
        }
    } else if (self->phase_ == 4) {
        for (int i = 0; i < 48; ++i) {
            func_ov017_021d22d4(self->player_, data_ov017_021d83d4.offset_, self->data_);
            data_ov017_021d83d4.offset_ += 14;
            if (data_ov017_021d83d4.offset_ >= 736) break;
        }
        self->phase_ = 5;
    } else if (self->phase_ == 5) {
        if (CheckSlotsAllFree0205e488(queue)) {
            if (data_ov017_021d83d4.offset_ >= 736) return 11;
            self->phase_ = 4;
        }
    }
    return self->result_;
}
