#include <globaldefs.h>
#include "GameState/GameState.h"
#include "System/Graphics.h"

struct HeadNode02046b24;
struct Obj0204724C;
struct StructAt020473c8;
struct BitField0203402c;
struct Vec3Target0203a46c;
struct Struct020478bc;
struct SearchStruct0202c1a4;
struct Vec3copy0202ec84 { int x, y, z; };

struct Flagged0219ad84 { unsigned char padding_0[2]; unsigned char flag; };
struct GlobalFlags0219ad84 { unsigned char padding_0[0x998]; unsigned int flags; };
struct Sprite0219ad84 { unsigned char padding_0[0x84]; unsigned char active : 1; unsigned char padding_85[3]; };
struct UiState0219ad84 {
    unsigned char padding_0[0x2b90];
    Sprite0219ad84 sprites[20];
    Sprite0219ad84* current;
    unsigned char field_3634;
    unsigned char padding_3635[0x3646 - 0x3635];
    unsigned char field_3646;
    unsigned char padding_3647[0x36b4 - 0x3647];
    unsigned char kind;
    unsigned char index;
    unsigned char padding_36b6[0x36fc - 0x36b6];
    HeadNode02046b24** list;
    unsigned char padding_3700[0x3710 - 0x3700];
    Flagged0219ad84* entry_3710;
    unsigned char padding_3714[0x3720 - 0x3714];
    Flagged0219ad84* entry_3720;
    unsigned char padding_3724[0x3b00 - 0x3724];
    Flagged0219ad84* entry_3b00;
    unsigned char padding_3b04[0x3b0c - 0x3b04];
    Flagged0219ad84* entry_3b0c;
    unsigned char padding_3b10[0x3b14 - 0x3b10];
    Flagged0219ad84* entry_3b14;
    Flagged0219ad84* entry_3b18;
    unsigned char padding_3b1c[0x3b20 - 0x3b1c];
    Flagged0219ad84* entry_3b20;
    Flagged0219ad84* entry_3b24;
    Flagged0219ad84* entry_3b28;
    unsigned char padding_3b2c[0x3b3c - 0x3b2c];
    Flagged0219ad84* entry_3b3c;
    unsigned char padding_3b40[0x3b4c - 0x3b40];
    Flagged0219ad84* entry_3b4c;
    Flagged0219ad84* entry_3b50;
    unsigned char padding_3b54[0x3b68 - 0x3b54];
    Flagged0219ad84* entry_3b68;
    unsigned char padding_3b6c[0x3b70 - 0x3b6c];
    Flagged0219ad84* entry_3b70;
    Flagged0219ad84* entry_3b74;
    unsigned char padding_3b78[0x3ba4 - 0x3b78];
    Flagged0219ad84* entry_3ba4;
    Flagged0219ad84* entry_3ba8;
    unsigned char padding_3bac[0x3bb8 - 0x3bac];
    Flagged0219ad84* entry_3bb8;
};

int GetBitsInField4(unsigned int*, unsigned int);
extern "C" unsigned short* func_02012fe4();
void* GetField0x3b0Value(GameState*);
GlobalFlags0219ad84* GetGlobalField0x1c020421a0();
Flagged0219ad84* GetField0x3f8Address(GameState*);
int GetHeadNodeIdOrMinusOne(HeadNode02046b24**);
void AdvanceStreamRecord0204724c(Obj0204724C*);
void RenderFlaggedIndexedEntry(StructAt020473c8*, int);
unsigned char GetByte0x26c(char*);
int CheckField0xc4Low15BitsNonZero(BitField0203402c*);
void SetVec3At0x1c(Vec3Target0203a46c*, int, int, int);
void RecomputeField0x7c(Struct020478bc*, int);
extern "C" void func_02047554(Sprite0219ad84*, int, int);
int CheckSubstructByte0x7cPositive(signed char*);
int IsField0Null(void**);
extern "C" int* func_0202ae18();
int CheckField0NonZero(int*);
int GetSearchStructCurrentArrEntry(SearchStruct0202c1a4*);
GameObject* GetCombatantWithFlag0x100(GameState*, int);
extern "C" void _Z27ComputeTwoFromVec3_0202ec84PvP16Vec3copy0202ec84PiS2_(void*, Vec3copy0202ec84*, int*, int*);
int HasFlagBit_0218d4ec(void*, int);

// USA: func_ov017_0219ad84
extern "C" ARM void func_ov017_0219ad84(UiState0219ad84* self) {
    if (GetBitsInField4((unsigned int*)self, 0x40)) return;
    GameState* game = GameState::GetInstance();
    unsigned short* objectId = func_02012fe4();
    void* camera = GetField0x3b0Value(game);
    GlobalFlags0219ad84* global = GetGlobalField0x1c020421a0();
    Flagged0219ad84* entry = GetField0x3f8Address(game);
    GameObject* object = game->GetUnknownGameObject();
    if (object == 0) return;
    if (GetHeadNodeIdOrMinusOne(self->list) == 4) {
        Sprite0219ad84* current = self->current;
        if (current == 0) return;
        if (!current->active) return;
        GXFIFO_MATRIX_PUSH = 0;
        if (self->current != &self->sprites[12] && self->current != &self->sprites[13] && self->current != &self->sprites[14]) {
            AdvanceStreamRecord0204724c((Obj0204724C*)self->current);
            RenderFlaggedIndexedEntry((StructAt020473c8*)self->current, 1);
        } else {
            RenderFlaggedIndexedEntry((StructAt020473c8*)self->current, 1);
        }
        GXFIFO_MATRIX_POP = 1;
        return;
    }
    unsigned int busy = 0;
    if (global) busy |= global->flags;
    busy |= self->entry_3710->flag;
    busy |= self->entry_3b24->flag;
    busy |= self->entry_3720->flag;
    busy |= self->entry_3b4c->flag;
    busy |= self->entry_3b00->flag;
    busy |= self->entry_3b3c->flag;
    busy |= self->entry_3b18->flag;
    busy |= self->entry_3b28->flag;
    busy |= self->entry_3b68->flag;
    busy |= self->entry_3b0c->flag;
    busy |= self->entry_3bb8->flag;
    busy |= self->entry_3b20->flag;
    busy |= self->entry_3b14->flag;
    busy |= self->entry_3ba4->flag;
    busy |= self->entry_3b70->flag;
    busy |= self->entry_3b74->flag;
    busy |= self->entry_3b50->flag;
    busy |= self->entry_3ba8->flag;
    int idle = 1;
    busy |= GetByte0x26c((char*)object);
    busy |= CheckField0xc4Low15BitsNonZero((BitField0203402c*)object);
    busy |= entry->flag;
    if (busy) idle = 0;
    if (idle) {
        SetVec3At0x1c((Vec3Target0203a46c*)&self->sprites[0], self->field_3634 << 12, self->field_3646 << 12, -0x1000);
        RecomputeField0x7c((Struct020478bc*)&self->sprites[0], 1);
        GXFIFO_MATRIX_PUSH = 0;
        func_02047554(&self->sprites[0], 0, 1);
        GXFIFO_MATRIX_POP = 1;
    }
    if (self->index != 18 && !CheckSubstructByte0x7cPositive((signed char*)object) && IsField0Null((void**)self->list)) {
        int ok = 1;
        if (GetBitsInField4((unsigned int*)self, 8)) {
            if (self->kind == 0) ok = 0;
        }
        if (self->index == 17) ok = 0;
        if (ok) {
            Sprite0219ad84* slot = &self->sprites[self->index];
            GXFIFO_MATRIX_PUSH = 0;
            AdvanceStreamRecord0204724c((Obj0204724C*)slot);
            RenderFlaggedIndexedEntry((StructAt020473c8*)slot, 1);
            GXFIFO_MATRIX_POP = 1;
        }
    }
    int* search = func_0202ae18();
    if (!CheckField0NonZero(search) || camera == 0) return;
    if (CheckSubstructByte0x7cPositive((signed char*)object)) return;
    int skip = GetSearchStructCurrentArrEntry((SearchStruct0202c1a4*)search);
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if (i == skip) continue;
        GameObject* c = GetCombatantWithFlag0x100(game, i);
        if (c == 0) continue;
        if (!c->obj3D_.IsVisible()) continue;
        if (!c->obj3D_.GetInheritedAlpha()) continue;
        int moving = 0;
        if (c->obj3D_.GetFlag(0x800000)) {
            if (c->obj3D_.GetFlag(0x400)) moving = 1;
        }
        unsigned short id = *objectId;
        if (id != c->obj3D_.GetField06() && !moving) continue;
        Vector3fix position = c->obj3D_.position_;
        position.y += c->obj3D_.GetHeight();
        int x, y;
        _Z27ComputeTwoFromVec3_0202ec84PvP16Vec3copy0202ec84PiS2_(camera, (Vec3copy0202ec84*)&position, &x, &y);
        x -= 12;
        y -= 24;
        if (c->obj3D_.GetFlag(0x400)) {
            SetVec3At0x1c((Vec3Target0203a46c*)&self->sprites[18], x << 12, y << 12, -0xa000);
            RecomputeField0x7c((Struct020478bc*)&self->sprites[18], 1);
            AdvanceStreamRecord0204724c((Obj0204724C*)&self->sprites[18]);
            GXFIFO_MATRIX_PUSH = 0;
            RenderFlaggedIndexedEntry((StructAt020473c8*)&self->sprites[18], 1);
            count++;
            GXFIFO_MATRIX_POP = 1;
            if (count >= 3) break;
        } else if (*(unsigned short*)((char*)c + 0x1b2)) {
            SetVec3At0x1c((Vec3Target0203a46c*)&self->sprites[19], x << 12, y << 12, -0xa000);
            GXFIFO_MATRIX_PUSH = 0;
            AdvanceStreamRecord0204724c((Obj0204724C*)&self->sprites[19]);
            RenderFlaggedIndexedEntry((StructAt020473c8*)&self->sprites[19], 1);
            count++;
            GXFIFO_MATRIX_POP = 1;
            if (count >= 3) break;
        } else {
            if (!HasFlagBit_0218d4ec(self, c->obj3D_.unknown_4_)) continue;
            SetVec3At0x1c((Vec3Target0203a46c*)&self->sprites[17], x << 12, y << 12, -0xa000);
            GXFIFO_MATRIX_PUSH = 0;
            AdvanceStreamRecord0204724c((Obj0204724C*)&self->sprites[17]);
            RenderFlaggedIndexedEntry((StructAt020473c8*)&self->sprites[17], 1);
            GXFIFO_MATRIX_POP = 1;
        }
    }
}
