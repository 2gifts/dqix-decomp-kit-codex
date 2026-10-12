#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Vector.h"

#if defined(jpn)
enum { RegionOffset6c = 0x8c };
#else
enum { RegionOffset6c = 0x6c };
#endif

struct PathNode_021975e4 {
    char pad0[8];
    Vector3i position;
    char pad14[0xc];
    short angle;
    char pad22[0xc];
    unsigned short partnerId;
    char pad30;
    unsigned char flags;
    char pad32[0x3e];
    PathNode_021975e4* next;
};

struct Walker_021975e4 {
    Object3D obj;
    char padac[2];
    short heading;
    char padb0[0xe];
    unsigned char mode;
};

struct TurnState_021975e4 {
    char pad0[0x42ee];
    short holdFrames;
};

struct SubBlock02038508 {
    unsigned char active;
    unsigned char unknown_1;
    unsigned char unknown_2;
    unsigned char reversed;
    unsigned short unknown_4;
    short angle;
    short startAngle;
    short endAngle;
    Vector3i start;
    Vector3i end;
    Vector3i unknown_24;
    Vector3i startControl;
    Vector3i endControl;
    int unknown_48;
    int unknown_4c;
    short unknown_50;
    unsigned char unknown_52;
    unsigned char unknown_53;
    unsigned char unknown_54;
    PathNode_021975e4* from;
    PathNode_021975e4* to;
    int unknown_60;
};

struct BitField0203402c;
struct ListHead02046b60;
struct NodeOwner_1e6d4;

extern unsigned short data_02114e30;
extern "C" void* func_02012fe4(void);
unsigned int* GetDataPtr02114e04_020d6c00();
SubBlock02038508* GetFieldPtrAt0x26c(void* obj);
int TestFlagMask(unsigned short* flags, int mask);
int CheckSubstructByte0x7cPositive(signed char* obj);
int CheckField0xc4Low15BitsNonZero(BitField0203402c* obj);
int ListContainsId(ListHead02046b60* list, int id);
void* GetPointerFromArray0x3c(unsigned char* obj, unsigned int index);
void InitBlock02038508(SubBlock02038508* block);
extern "C" int func_02094b9c(PathNode_021975e4* node, Vector3i* point);
PathNode_021975e4* FindNodeKind9ById(NodeOwner_1e6d4* owner, int id);
extern "C" void _Z16CopyBlockTo0x26cP13Owner020398b4P16SubBlock020398b4(Walker_021975e4* obj, SubBlock02038508* block);
void OrBitsIntoField0(unsigned int* field, unsigned int bits);

// USA: func_ov017_021975e4
extern "C" ARM void func_ov017_021975e4(TurnState_021975e4* state) {
    GameState* game = GameState::GetInstance();
    unsigned char* zone = (unsigned char*)func_02012fe4();
    unsigned int* flags = GetDataPtr02114e04_020d6c00();
    Walker_021975e4* player = (Walker_021975e4*)game->GetUnknownGameObject();
    Vector3i anchor = player->obj.position_;
    SubBlock02038508* current = GetFieldPtrAt0x26c(player);
    GameResources* res = func_ov017_0218b5b0();
    ListHead02046b60* list = (ListHead02046b60*)res->unknown_ptr_array_36fc[0];

    if (TestFlagMask(&data_02114e30, 0x402)) return;
    if (CheckSubstructByte0x7cPositive((signed char*)player) || current->active) return;
    if (CheckField0xc4Low15BitsNonZero((BitField0203402c*)player)) return;
    if (ListContainsId(list, 0xc)) return;
    if (ListContainsId(list, 0x2d)) return;
    if (ListContainsId(list, 0x26)) return;
    if (ListContainsId(list, 0x35)) return;
    int idle = player->mode == 0 ? 1 : 0;
    if (idle == 1) return;

    PathNode_021975e4* node = (PathNode_021975e4*)GetPointerFromArray0x3c(zone + RegionOffset6c, 9);
    while (node != 0) {
        SubBlock02038508 block;
        InitBlock02038508(&block);
        if (func_02094b9c(node, &anchor)) {
            Vector3i nodeDir;
            int cosine = fix32cos(node->angle);
            nodeDir.x = fix32sin(node->angle);
            nodeDir.y = 0;
            nodeDir.z = cosine;
            Vector3fix_Normalize(&nodeDir, &nodeDir);

            short heading = player->heading;
            Vector3i playerDir;
            cosine = fix32cos(heading);
            playerDir.x = fix32sin(heading);
            playerDir.y = 0;
            playerDir.z = cosine;
            Vector3fix_Normalize(&playerDir, &playerDir);

            Vector3i facing = *(Vector3i*)((char*)res + 0x4438);
            int alignment = Vector3fix_InnerProduct(&nodeDir, &playerDir);
            int lateral = Vector3fix_InnerProduct(&nodeDir, &facing);
            Vector3i offset;
            Vector3fix_Subtract(&anchor, &node->position, &offset);

            if (node->flags & 1) {
                if (alignment >= 0) {
                    node = node->next;
                    continue;
                }
            } else {
                if (alignment <= 0) {
                    node = node->next;
                    continue;
                }
            }

            int ready = 0;
            if (state->holdFrames <= 1) {
                if (player->mode == 1 && (lateral < -0xccc || lateral > 0xccc || alignment < -0xccc || alignment > 0xccc)) {
                    state->holdFrames++;
                } else {
                    state->holdFrames = 0;
                }
            } else if (state->holdFrames > 1) {
                state->holdFrames = 0;
                ready = 1;
            }
            if (!ready) {
                node = node->next;
                continue;
            }

            PathNode_021975e4* partner = FindNodeKind9ById((NodeOwner_1e6d4*)(zone + RegionOffset6c), node->partnerId);
            if (!partner) return;

            block.active = 1;
            block.angle = node->angle;
            if (node->flags & 1) {
                block.to = node;
                block.from = partner;
                block.start = partner->position;
                block.end = node->position;
                block.startControl = partner->position;
                block.endControl = node->position;
                block.startAngle = partner->angle;
                block.endAngle = node->angle;
                block.reversed = 1;
                block.endControl.x += nodeDir.x;
                block.endControl.z += nodeDir.z;
                block.startControl.x += -nodeDir.x >> 2;
                block.startControl.z += -nodeDir.z >> 2;
                block.end.y -= 0x1000;
                block.startAngle -= 0x3243;
                if (block.startAngle < 0) block.startAngle += 0x6487;
            } else {
                block.to = partner;
                block.from = node;
                block.start = node->position;
                block.end = partner->position;
                block.startControl = node->position;
                block.endControl = partner->position;
                block.startAngle = node->angle;
                block.endAngle = partner->angle;
                block.reversed = 0;
                block.endControl.x += nodeDir.x;
                block.endControl.z += nodeDir.z;
                block.startControl.x += -nodeDir.x >> 2;
                block.startControl.z += -nodeDir.z >> 2;
                block.end.y -= 0x1000;
                block.startAngle -= 0x3243;
                if (block.startAngle < 0) block.startAngle += 0x6487;
            }
            _Z16CopyBlockTo0x26cP13Owner020398b4P16SubBlock020398b4(player, &block);
            OrBitsIntoField0(flags, 0x400);
        }
        node = node->next;
    }
}
