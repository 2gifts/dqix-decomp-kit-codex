#include <globaldefs.h>
#include <std_library_functions.h>
#include "GameState/GameState.h"
#include "System/Memory.h"

void* GetData02100044();
extern "C" void func_0205e330(void* dispatcher, void* packet, int flag);
extern "C" void* func_0202ae18(void);
int GetFieldAt0x0(int* obj);
int UpdatePlayClocks020ac4f8(int commit);
int LoadBattleBlock020ac4c0(void* dst);
int CopyOutBattleField0x7ac0(void* dst);
struct S_a0870;
struct S_a08a4;
struct S_a08d8;
struct S_a090c;
int GetFieldPercentOver307(struct S_a0870* p);
int GetFieldPercentOver232(struct S_a08a4* p);
int GetFieldPercentOver944(struct S_a08d8* p);
int GetFieldPercentOver448(struct S_a090c* p);

struct PacketIdentifier { unsigned char bytes[6]; };

struct PacketBody {
    unsigned char payload[14];
    unsigned char recipient : 2;
    unsigned char mode : 2;
    unsigned char field_12 : 4;
    unsigned char type : 2;
    unsigned char length : 4;
    unsigned char final : 1;
    unsigned char segmented : 1;
};

struct Packet {
    unsigned char tag;
    char pad_1[3];
    PacketBody body;
};

struct BattleBlock {
    unsigned short f00;
    unsigned char f02;
    unsigned char f03;
    unsigned short f04;
    unsigned char f06;
    unsigned char f07;
    unsigned int f08 : 24;
    unsigned int f08hi : 8;
    unsigned int f0cA : 14;
    unsigned int f0cB : 14;
    unsigned int f0cC : 4;
    unsigned int f10A : 9;
    unsigned int f10B : 14;
    unsigned int f10C : 9;
    char rest[0xb0 - 0x14];
};

struct BattleRecord {
    unsigned int w0A : 14;
    unsigned int w0B : 17;
    unsigned int w0C : 1;
    unsigned int w4A : 14;
    unsigned int w4B : 17;
    unsigned int w4C : 1;
    unsigned int w8A : 7;
    unsigned int w8B : 7;
    unsigned int w8C : 7;
    unsigned int w8D : 7;
    unsigned int w8E : 4;
    unsigned int wcA : 10;
    unsigned int wcB : 14;
    unsigned int wcC : 7;
    unsigned int wcD : 1;
    unsigned int w10A : 9;
    unsigned int w10B : 14;
    unsigned int w10C : 7;
    unsigned int w10D : 2;
    short f14;
    unsigned char protagonistFlag : 1;
    unsigned char protagonistValue : 4;
};

struct LinkBlock {
    unsigned int w0;
    unsigned int lo : 19;
    signed int val : 11;
    unsigned int hi : 2;
    char rest[0x7c - 8];
};

struct LinkState {
    char pad_0[0x569c];
    LinkBlock block;
    char pad_5718[0x74fe - 0x5718];
    PacketIdentifier identifier;
    PacketIdentifier GetIdentifier() { return identifier; }
};

struct ProtagonistData {
    char pad_0[0x186];
    unsigned char values[0x49c - 0x186];
    unsigned char flag : 1;
    char pad_49d[0x950 - 0x49d];
    int index;
};

struct ProtagonistObject {
    char pad_0[0x150];
    ProtagonistData* data;
};

struct Session {
    char pad_0[0x100d];
    unsigned char depth;
};

// USA: func_ov017_021d1118
extern "C" ARM void func_ov017_021d1118(int recipient, int flags, bool segmented, unsigned char mode) {
    GameState* gs;
    void* dispatcher;
    PacketBody* body;
    unsigned char* src;
    Session* session;
    int kind;
    int depth;
    int commit;
    int flag;
    int value;
    ProtagonistObject* protagonist;
    ProtagonistData* data;
    int recordLeft;
    int blockLeft;
    LinkBlock* block;
    gs = GameState::GetInstance();
    dispatcher = GetData02100044();
    Packet packet;
    body = &packet.body;
    packet.tag = 0x1a;
    body->mode = mode;
    body->segmented = segmented;
    body->recipient = recipient;
    block = &((LinkState*)gs)->block;
    if (flags & 4) {
        body->type = 2;
        body->length = 6;
        PacketIdentifier identifier = ((LinkState*)gs)->GetIdentifier();
        memcpy(body->payload, &identifier, body->length);
        func_0205e330(dispatcher, &packet, 0);
    }
    if (flags & 1) {
        session = (Session*)func_0202ae18();
        kind = GetFieldAt0x0((int*)session);
        depth = session->depth;
        commit = 0;
        if ((kind == 5 && depth > 1) || kind == 6) commit = 1;
        UpdatePlayClocks020ac4f8(commit);
        BattleBlock blk;
        LoadBattleBlock020ac4c0(&blk);
        unsigned int score = 0;
        CopyOutBattleField0x7ac0(&score);
        flag = 0;
        value = 0;
        protagonist = (ProtagonistObject*)gs->GetProtagonist();
        if (protagonist) {
            data = protagonist->data;
            flag = data->flag;
            value = data->values[data->index];
        }
        BattleRecord rec;
        VectorizedMemset(&rec, 0, sizeof(rec));
        rec.w0A = blk.f00;
        rec.w8A = blk.f02;
        rec.w4A = blk.f04;
        rec.w8B = blk.f06;
        rec.w4B = blk.f08;
        rec.w0B = score;
        rec.wcA = blk.f10A;
        rec.w10A = blk.f08hi;
        rec.wcB = blk.f0cB;
        rec.w10B = blk.f10B;
        rec.w8C = (unsigned char)GetFieldPercentOver307((struct S_a0870*)&blk);
        rec.w8D = (unsigned char)GetFieldPercentOver232((struct S_a08a4*)&blk);
        rec.wcC = (unsigned char)GetFieldPercentOver944((struct S_a08d8*)&blk);
        rec.w10C = (unsigned char)GetFieldPercentOver448((struct S_a090c*)&blk);
        rec.f14 = block->val;
        rec.protagonistFlag = flag;
        rec.protagonistValue = value;
        body->type = 0;
        body->final = 0;
        src = (unsigned char*)&rec;
        for (recordLeft = sizeof(rec); recordLeft > 0; src += 14, recordLeft -= 14) {
            if (recordLeft > 14) {
                body->length = 14;
            } else {
                body->length = recordLeft;
                if (!(flags & 2)) body->final = 1;
            }
            memcpy(body->payload, src, body->length);
            func_0205e330(dispatcher, &packet, 0);
        }
    }
    if (flags & 2) {
        body->type = 1;
        src = (unsigned char*)block;
        for (blockLeft = sizeof(*block); blockLeft > 0; src += 14, blockLeft -= 14) {
            if (blockLeft > 14) {
                body->length = 14;
            } else {
                body->length = blockLeft;
                body->final = 1;
            }
            memcpy(body->payload, src, body->length);
            func_0205e330(dispatcher, &packet, 0);
        }
    }
}
