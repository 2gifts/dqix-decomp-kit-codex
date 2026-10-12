#include <globaldefs.h>
#include "GameState/GameState.h"

struct SearchStruct0202c1a4;
struct ArrayContainsByteStruct;

struct Struct021c316c {
    unsigned char byte0;
    unsigned char byte1;
    unsigned char busy;
};

struct Entry02192700 {
    unsigned short tag;
    unsigned short group : 2;
};

struct Ctx02192700 {
    unsigned int flags;
    char pad4[0x36f8 - 0x4];
    Entry02192700* entry;
    char pad36fc[0x3bb4 - 0x36fc];
    Struct021c316c* state;
};

struct Actor02192700 {
    Object3D obj3D;
    short heightOffset;
    char padae[0xb2 - 0xae];
    unsigned short fieldb2;
    char padb4[0xc2 - 0xb4];
    unsigned char nearMask : 4;
    char padc3[0x18c - 0xc3];
    unsigned int flags18c;
};

void InitObj0219a674(unsigned char* self);

struct Msg02192700 {
    unsigned char kind;
    unsigned char field1;
    short field2;
    unsigned char field4;
    char pad5[3];
    int field8;
    int fieldc;
    short angle;
    short field12;
};

struct Save0205ec34 {
    char pad0[0x8c];
    unsigned char flags[1];
};

extern unsigned short data_02114e30;

unsigned int GetBitsInField0(unsigned int* obj, unsigned int mask);
int TestFlagMask(unsigned short* flags, int mask);
extern "C" SearchStruct0202c1a4* func_0202ae18(void);
extern "C" Save0205ec34* func_0205ec34(void);
struct Entry_02028bd0* GetEntryTableBase(void);
int CheckField0NonZero(int* p);
signed char GetSearchStructCurrentArrEntry(SearchStruct0202c1a4* obj);
extern "C" int func_ov017_02194140(Actor02192700* self, Actor02192700* other);
int CheckFlag0x1ceBit2Set(unsigned char* obj);
void* GetPtrField0x2a04(GameState* state);
int ArrayContainsByte(ArrayContainsByteStruct* arr, int value);
extern "C" void func_ov017_0219b33c(Ctx02192700* self, Msg02192700* msg);
void InitState71WithFlag_021c316c(Struct021c316c* p, int flag);
int HasFlagBit_0218d4ec(void* obj, int bit);
int TestBitInByteArray(int base, unsigned char* bits, int index);
int GetSignedByte0x1c8(void* obj);
extern "C" void func_ov017_0219344c(Actor02192700* actor, int a, int b, int c, int d, int e);

static inline Ctx02192700* PostFound() {
    Ctx02192700* res = (Ctx02192700*)func_ov017_0218b5b0();
    Msg02192700 msg;
    InitObj0219a674((unsigned char*)&msg);
    msg.kind = 1;
    func_ov017_0219b33c(res, &msg);
    return res;
}

static inline void InitNotice(Msg02192700* msg) {
    InitObj0219a674((unsigned char*)msg);
    msg->kind = 0xd;
    msg->field1 = 0x11;
}

static inline void PostNotice(Ctx02192700* res, Actor02192700* member) {
    Msg02192700 msg;
    InitNotice(&msg);
    msg.field2 = member->obj3D.unknown_4_;
    func_ov017_0219b33c(res, &msg);
}

// USA: func_ov017_02192700
extern "C" ARM void func_ov017_02192700(Ctx02192700* ctx) {
    if ((GetBitsInField0(&ctx->flags, 0x20) != 0 && TestFlagMask(&data_02114e30, 1) != 0) ||
        GetBitsInField0(&ctx->flags, 0x2000) != 0) {
        return;
    }
    GameState* game = GameState::GetInstance();
    SearchStruct0202c1a4* search = func_0202ae18();
    Save0205ec34* save = func_0205ec34();
    Actor02192700* leader = (Actor02192700*)game->GetUnknownGameObject();
    if (leader == NULL) {
        return;
    }
    unsigned short leaderMap = leader->obj3D.GetField06();
    GetEntryTableBase();
    Entry02192700* entry = ctx->entry;
    if (CheckField0NonZero((int*)search)) {
        for (int i = 0; i <= 3; i++) {
            Actor02192700* member = (Actor02192700*)game->GetPartyMemberByIndex(i);
            if (member == NULL || member == leader) {
                continue;
            }
            if (leaderMap != member->obj3D.GetField06()) {
                continue;
            }
            if ((member->flags18c & 1) && member->obj3D.GetInheritedAlpha() > 0) {
                if (!func_ov017_02194140(leader, member)) {
                    continue;
                }
                GameState* state = GameState::GetInstance();
                Actor02192700* other = (Actor02192700*)state->GetPartyMemberByIndex(member->obj3D.unknown_4_);
                if (other->obj3D.GetField06() != member->obj3D.GetField06()) {
                    continue;
                }
                if (other->obj3D.GetInheritedAlpha() <= 0) {
                    continue;
                }
                if (other->obj3D.GetAlphaScaleFactor() <= 0) {
                    continue;
                }
                if (!other->obj3D.IsVisible()) {
                    continue;
                }
                if (CheckFlag0x1ceBit2Set((unsigned char*)other)) {
                    continue;
                }
                ArrayContainsByteStruct* party = (ArrayContainsByteStruct*)GetPtrField0x2a04(state);
                if (party == NULL) {
                    continue;
                }
                if (ArrayContainsByte(party, member->obj3D.unknown_4_)) {
                    continue;
                }
                Ctx02192700* res = PostFound();
                if (res->state->busy == 0) {
                    InitState71WithFlag_021c316c(res->state, member->obj3D.unknown_4_ & 0xff);
                }
                continue;
            }
            if (HasFlagBit_0218d4ec(ctx, member->obj3D.unknown_4_) &&
                TestBitInByteArray((int)save, save->flags, 0x799) &&
                func_ov017_02194140(leader, member)) {
                PostNotice((Ctx02192700*)func_ov017_0218b5b0(), member);
            }
            if (GetSignedByte0x1c8(member) >= 0) {
                continue;
            }
            if (!member->obj3D.IsVisible()) {
                continue;
            }
            int id = member->obj3D.unknown_4_;
            if (func_ov017_02194140(leader, member)) {
                if (!(leader->nearMask & (1 << id))) {
                    leader->nearMask |= 1 << id;
                    leader->fieldb2 = 0;
                }
            } else {
                leader->nearMask &= ~(1 << id);
            }
        }
    }
    if (!CheckField0NonZero((int*)search) || !GetSearchStructCurrentArrEntry(search)) {
        for (int i = 0x70; i <= 0x9f; i++) {
            Actor02192700* monster = (Actor02192700*)GameState::GetInstance()->GetMaybeFieldMonsterByIndex(i);
            if (monster == NULL || monster->obj3D.unknown_2_ < 0) {
                continue;
            }
            Vector3i pos = monster->obj3D.position_;
            pos.y -= monster->heightOffset;
            if (pos.y < -0x1e000) {
                pos.y = 0x1e000;
            }
            monster->obj3D.position_ = pos;
            if (monster->obj3D.GetField06() == leader->obj3D.GetField06()) {
                func_ov017_0219344c(monster, 0, 0, 0, 0, 0);
            }
        }
    } else if (entry != NULL) {
        for (int i = 0; i <= 12; i++) {
            Actor02192700* monster = (Actor02192700*)GameState::GetInstance()->GetMaybeFieldMonsterByIndex(i + (entry->group * 12 + 0x70));
            if (monster == NULL || monster->obj3D.unknown_2_ < 0) {
                continue;
            }
            func_ov017_0219344c(monster, 0, 0, 0, 0, 0);
        }
    }
}
