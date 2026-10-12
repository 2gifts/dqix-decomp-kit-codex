#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Resource/GameResources.h"

struct CamNode_02197a64 {
    char pad00[8];
    Vector3i position;
    char pad14[0xc];
    short angle;
    char pad22[0xc];
    unsigned short kind : 4;
    unsigned short flags : 12;
    char pad30[0x70 - 0x30];
    struct CamNode_02197a64* next;
};

struct CamRequest_02197a64 {
    unsigned char type;
    char pad1[3];
    unsigned char count;
    char pad5[3];
    struct CamNode_02197a64* nodes[2];
    short angle;
};

struct CamPlayer_02197a64 { Object3D obj; char padac[0x12]; unsigned char mode; };
struct CamState_02197a64 { char pad4200[0xec]; short misses; };

extern "C" void* func_02012fe4();
void* GetPointerFromArray0x3c(unsigned char*, unsigned int);
int GetField4328_0218d268(void*);
void InitObj0219a674(unsigned char*);
extern "C" int func_02094b9c(void*, void*);
int BuildAndForwardVec_0218da48(void*, int, int, int, unsigned char);
int CheckAndComputeDistance020968d0(void*, int*);
int TestFlagMask(unsigned short*, int);
struct Vec3;
int GetAbsAngleDeltaBetween_021a4754(Vec3*, int, Vec3*);
extern "C" void func_ov017_0219b33c(void*, void*);
extern "C" void func_ov017_02198f84(void*);
extern unsigned short data_02114e30;

// USA: func_ov017_02197a64
extern "C" ARM void func_ov017_02197a64(GameResources* res) {
    GameResources* resources;
    GameState* game = GameState::GetInstance();
    resources = func_ov017_0218b5b0();
    unsigned char* zone = (unsigned char*)func_02012fe4();
    CamPlayer_02197a64* player = (CamPlayer_02197a64*)game->GetUnknownGameObject();
    Vector3i anchor = player->obj.position_;
    CamNode_02197a64* node = (CamNode_02197a64*)GetPointerFromArray0x3c(zone + 0x6c, 2);
    int distObj = GetField4328_0218d268(resources);
    int forward = 0;
    CamRequest_02197a64 request;
    InitObj0219a674((unsigned char*)&request);
    int handle = 0;
    int missed = 0;
    CamState_02197a64* state = (CamState_02197a64*)((char*)res + 0x4200);
    while (node != 0 && request.count < 2) {
        short* missCount = (short*)((char*)res + 0x42ec);
        if ((node->flags & 1) == 0 && (node->flags & 4) == 0 && func_02094b9c(node, &anchor)) {
            if (handle == 0) {
                handle = BuildAndForwardVec_0218da48(res, node->angle, 0xccc, 0xccc >> 3, 0);
            }
            Vector3i dirA;
            int cosA = fix32cos(node->angle);
            dirA.x = fix32sin(node->angle);
            dirA.y = 0;
            dirA.z = cosA;
            Vector3fix_Normalize(&dirA, &dirA);
            Vector3i dirB;
            int cosB = fix32cos(player->obj.rotation_.y);
            dirB.x = fix32sin(player->obj.rotation_.y);
            dirB.y = 0;
            dirB.z = cosB;
            Vector3fix_Normalize(&dirB, &dirB);
            Vector3i velocity = *(Vector3i*)((char*)resources + 0x4438);
            int side = Vector3fix_InnerProduct(&dirA, &dirB);
            int along = Vector3fix_InnerProduct(&dirA, &velocity);
            if ((velocity.x != 0 || velocity.z != 0) && (CheckAndComputeDistance020968d0((void*)distObj, 0) || TestFlagMask(&data_02114e30, 0xf0))) side = along;
            if (node->flags & 0x10) {
                Vector3i toNode;
                Vector3fix_Subtract(&anchor, &node->position, &toNode);
                toNode.y = 0;
                Vector3fix_Normalize(&toNode, &toNode);
                if (Vector3fix_InnerProduct(&dirA, &toNode) > 0) {
                    if (side >= 0) {
                        node = node->next;
                        continue;
                    }
                } else {
                    if (side <= 0) {
                        node = node->next;
                        continue;
                    }
                }
            } else if (side > 0) {
                node = node->next;
                continue;
            }
            request.nodes[request.count++] = node;
            if (state->misses <= 2) {
                int lim = 0xccc;
                if (player->mode == 1 && (along < 0x170 - lim || along > lim - 0x170 || side < 0x170 - lim || side > lim - 0x170)) {
                    if (missed == 0) {
                        missed = 1;
                        (*missCount)++;
                    }
                } else {
                    state->misses = 0;
                }
            } else if (state->misses > 2) {
                if (missed == 0) forward = 1;
            }
        }
        node = node->next;
    }
    if (request.count != 0) {
        Vector3i sum = {0, 0, 0};
        int i = 0;
        for (; i < request.count; i++) {
            Vector3fix_Add(&sum, &request.nodes[i]->position, &sum);
        }
        Vector3fixDivideScalar(&sum, request.count << 12, &sum);
        request.angle = GetAbsAngleDeltaBetween_021a4754((Vec3*)&player->obj.position_, (short)player->obj.rotation_.y, (Vec3*)&sum);
        request.type = 4;
        func_ov017_0219b33c(res, &request);
    }
    if (forward) func_ov017_02198f84(res);
}
