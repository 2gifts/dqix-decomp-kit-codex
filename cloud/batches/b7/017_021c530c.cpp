#include <globaldefs.h>
#include "GameState/GameState.h"

extern "C" void __clear(void* buf, int size);
int CollectCombatantsWithFlag0x1000(GameState* battleStruct, unsigned char* out);
extern "C" void _Z34SetField0x1b2IfMatchOrFlag02053f7cP11Obj02053f7csi(GameObject* obj, int a, int b);
extern "C" void* func_0202ae18(void);
extern "C" int func_0202c508(void* search);
void EnqueueEventTag147_021cdaa0(void);
int CheckByteAtIndex_021c5bf8(int index, int key);
extern "C" void _Z24MaybeResetEntry_021c5e28is(int id, int val);
extern "C" void func_ov017_021c5a8c(int a, int key);
int IsValidCombatantValue_021c5a1c(int val);
void* GetData02108ea8(void);
extern "C" int func_0207d7f4(void* entries, unsigned short id, int charId, int flag,
                             signed char field4, unsigned short field2, unsigned char field5);
extern "C" int func_ov017_021c5c5c(int index, int id, int arg);
extern "C" void func_ov017_021c5b44(unsigned char arg0, int combatantId, unsigned int arg2, unsigned char arg3, unsigned char arg4);
extern "C" void func_0207dcf0(unsigned short id);
GameObject* FindCombatantByField16a_021a278c(void* base, int val);
void TrySetMode02076ccc(void* obj, int mode);
struct Event021c530c;
extern "C" Event021c530c* func_ov017_021b8478(void* node);
int IsField0Null(void** list);
struct Src021c5f00 { unsigned short w0; unsigned short w2; unsigned char b4; unsigned char b5; unsigned char b6; };
void EnqueueEventTag63_021c5f00(Src021c5f00* src);
struct ListHead02046b38;
struct ListNode02046b38;
int ListContainsNode(struct ListHead02046b38* list, struct ListNode02046b38* target);
extern "C" void func_ov017_021b6f18(void* node);
void InitStruct_02196c08(unsigned char* obj);
extern "C" bool func_ov017_021b6e70(void* obj, int combatantId);
extern "C" void func_ov017_021b7104(void* node, void* src);
struct TailList020469b4;
struct TailNode020469b4;
void AppendNodeToTail(struct TailList020469b4* list, struct TailNode020469b4* node);
extern "C" void func_ov017_021a7a74(void* unused, void* src);
int HasMatchingHalfword_0219735c(unsigned char* obj, unsigned short key);
void EnqueueHalfword_021972f8(void* obj, unsigned short val);
struct ListHead02046b60;
int ListContainsId(struct ListHead02046b60* list, int id);
unsigned short GetFieldA_021bb9f4(void* obj);
short GetField14e_021bb9fc(void* obj);
void SetFieldA_021bb9ec(void* obj, unsigned short v);
extern "C" void* func_02012fe4(void);

struct Cmd021c530c {
    unsigned char pad0[4];
    unsigned short id;
    unsigned short field6;
    unsigned char field8;
    unsigned char field9;
    unsigned short fielda;
    unsigned char fieldc;
    unsigned char fieldd;
    unsigned short fielde;
    unsigned char field10;
};

struct Event021c530c {
    unsigned char pad0[8];
    unsigned short targetId;
};

struct Node021c530c {
    unsigned char pad0[2];
    unsigned char active;
};

struct Leader021c530c {
    Object3D obj3D;
    unsigned char padAc[0xb2 - 0xac];
    unsigned short fieldB2;
    unsigned char padB4[0xc3 - 0xb4];
    unsigned char fieldC3;
};

struct Pending021c530c {
    unsigned short id;
    unsigned short value;
};

struct Ctx021c530c {
    unsigned char pad0[0x36fc];
    void* list6fc;
    unsigned char pad3700[0x3718 - 0x3700];
    Node021c530c* node718;
    unsigned char pad371c[0x3734 - 0x371c];
    void* obj734;
    unsigned char pad3738[0x3b14 - 0x3738];
    void* nodeB14;
    unsigned char pad3b18[0x3b90 - 0x3b18];
    void* nodeB90;
    unsigned char pad3b94[0x44ae - 0x3b94];
    Pending021c530c pending;
};

struct Desc021c530c {
    unsigned char field0;
    unsigned char field1;
    unsigned short field2;
    unsigned short field4;
    unsigned char pad6[2];
    int field8;
    unsigned char fieldc;
    unsigned char fieldd;
    unsigned char fielde;
    unsigned char fieldf;
    short field10;
    unsigned char field12;
    unsigned char pad13;
    int field14;
};

struct Zone021c530c {
    unsigned short id;
};

static inline int IsSpecialId(int id) {
    return id >= 0x8005 && id <= 0x8005 + 1000;
}

static inline unsigned short GetZoneId(Zone021c530c* zone) {
    return zone->id;
}

// USA: func_ov017_021c530c
extern "C" ARM void func_ov017_021c530c(int index, Cmd021c530c* cmd, GameState* gs, unsigned char* work, void* search) {
    unsigned char members[4] = {0};
    GameObject* member;
    bool ok;
    GameObject* resetTarget;
    int id;
    Leader021c530c* leader;
    Ctx021c530c* ctx1;
    Node021c530c* node1;
    void* list1;
    unsigned short zone;
    Ctx021c530c* ctx2;
    Leader021c530c* leader2;
    GameObject* modeTarget;
    int plainId;
    int normal;
    Ctx021c530c* ctx3;
    void* list3;
    int found;
    GameObject* combatant;
    Zone021c530c* zoneInfo;
    int inB14;
    int inB90;
    Pending021c530c* pending;
    int count;
    int i;
    void* entries;
    int state;
    int queued;
    void* fieldObj;
    unsigned char field8;
    unsigned short cid;
    short fielda;
    unsigned char fieldc;
    unsigned char field10;
    void* list2;
    void* node2;

    members[0] = index;
    count = 1;
    if (index == 0) {
        count = CollectCombatantsWithFlag0x1000(gs, members);
    }
    for (i = 0; i < count; i++) {
        member = gs->GetPartyMemberByIndex(members[i]);
        if (member != NULL) {
            _Z34SetField0x1b2IfMatchOrFlag02053f7cP11Obj02053f7csi(member, cmd->id, 1);
        }
    }

    if (func_0202c508(search)) {
        EnqueueEventTag147_021cdaa0();
        if (!IsSpecialId(cmd->id)) {
            if (cmd->field9 != 0) {
                if (CheckByteAtIndex_021c5bf8(index, cmd->id) == 0) {
                    _Z24MaybeResetEntry_021c5e28is(index, cmd->id);
                } else {
                    func_ov017_021c5a8c(index, cmd->id);
                }
            } else if (IsValidCombatantValue_021c5a1c(cmd->id)) {
                if (CheckByteAtIndex_021c5bf8(index, cmd->id) == 0) {
                    if (!func_0202c508(func_0202ae18())) {
                        ok = false;
                    } else {
                        entries = GetData02108ea8();
                        func_0207d7f4(entries, cmd->id, index, 1, index, cmd->field6, cmd->fieldc);
                        ok = func_ov017_021c5c5c(index, cmd->id, index) != 0;
                        func_0207d7f4(entries, cmd->id, 0, ok, index, cmd->field6, cmd->fieldc);
                    }
                    if (ok) {
                        func_ov017_021c5b44(cmd->field8, cmd->id, -1, cmd->fieldc, cmd->field10);
                    }
                    func_0207dcf0(cmd->id);
                    resetTarget = FindCombatantByField16a_021a278c(work, cmd->id);
                    if (resetTarget != NULL) {
                        TrySetMode02076ccc(resetTarget, 9);
                    }
                } else {
                    func_ov017_021c5a8c(index, cmd->id);
                }
            } else {
                _Z24MaybeResetEntry_021c5e28is(index, cmd->id);
            }
        } else {
            func_ov017_021c5a8c(index, cmd->id);
        }
        return;
    }

    if (cmd->fieldd != 0) {
        return;
    }

    id = cmd->id;
    if (!IsSpecialId(cmd->id)) {
        if (cmd->field9 != 0) {
            Desc021c530c event;
            leader = (Leader021c530c*)GameState::GetInstance()->GetUnknownGameObject();
            if (leader == NULL) {
                state = 0;
            } else {
                ctx1 = (Ctx021c530c*)func_ov017_0218b5b0();
                node1 = ctx1->node718;
                list1 = ctx1->list6fc;
                if (node1->active != 0 && id == func_ov017_021b8478(node1)->targetId) {
                    state = 1;
                } else {
                    zone = leader->obj3D.GetField06();
                    if (zone != 0x170c && zone != 0x170d && zone != 0x170e && zone != 0x170f) {
                        state = 0;
                    } else if ((int)leader->fieldC3 > 0) {
                        state = 0;
                    } else if (IsField0Null((void**)list1) == 0) {
                        state = 2;
                    } else {
                        state = 1;
                    }
                }
            }

            Src021c5f00 src;
            src.w0 = cmd->id;
            src.w2 = cmd->field6;
            src.b5 = 0;
            src.b6 = 1;
            if (state == 1) {
                src.b4 = 1;
            } else {
                src.b4 = 0;
            }
            EnqueueEventTag63_021c5f00(&src);

            if (state == 1) {
                field10 = cmd->field10;
                fieldc = cmd->fieldc;
                fielda = cmd->fielda;
                cid = cmd->id;
                field8 = cmd->field8;
                ctx2 = (Ctx021c530c*)func_ov017_0218b5b0();
                list2 = ctx2->list6fc;
                node2 = ctx2->node718;
                if (!ListContainsNode((ListHead02046b38*)list2, (ListNode02046b38*)node2)) {
                    func_ov017_021b6f18(node2);
                    Desc021c530c desc;
                    InitStruct_02196c08((unsigned char*)&desc);
                    desc.field0 = 0;
                    desc.field1 = field8;
                    desc.field4 = cid;
                    desc.field2 = 0xffff;
                    desc.field8 = -1;
                    desc.fielde = 1;
                    desc.field10 = fielda;
                    desc.fieldf = fieldc;
                    func_ov017_021b6e70(&desc, desc.field2);
                    desc.field12 = field10;
                    func_ov017_021b7104(node2, &desc);
                    AppendNodeToTail((TailList020469b4*)list2, (TailNode020469b4*)node2);
                }
                leader2 = (Leader021c530c*)gs->GetUnknownGameObject();
                if (leader2 != NULL) {
                    leader2->fieldB2 = 0;
                }
            } else if (state == 2 || state == 0) {
                InitStruct_02196c08((unsigned char*)&event);
                event.field0 = 1;
                event.field1 = cmd->field8;
                event.field4 = cmd->id;
                event.field2 = 0xffff;
                event.field8 = -1;
                event.fielde = 1;
                event.field10 = cmd->fielda;
                func_ov017_021b6e70(&event, event.field2);
                if (state == 2) {
                    func_ov017_021a7a74(work, &event);
                }
            }
        } else if (IsValidCombatantValue_021c5a1c(id)) {
            if (!HasMatchingHalfword_0219735c(work, cmd->id)) {
                EnqueueHalfword_021972f8(work, cmd->id);
                queued = func_ov017_021c5c5c(index, cmd->id, cmd->field8);
                Src021c5f00 src;
                src.w0 = cmd->id;
                src.w2 = cmd->field6;
                src.b4 = queued;
                src.b5 = 0;
                src.b6 = 0;
                EnqueueEventTag63_021c5f00(&src);
                if (queued) {
                    func_ov017_021c5b44(cmd->field8, cmd->id, -1, cmd->fieldc, cmd->field10);
                } else {
                    modeTarget = FindCombatantByField16a_021a278c(work, cmd->id);
                    if (modeTarget != NULL) {
                        TrySetMode02076ccc(modeTarget, 9);
                    }
                }
            }
        } else {
            if (!HasMatchingHalfword_0219735c(work, cmd->id)) {
                EnqueueHalfword_021972f8(work, cmd->id);
            }
            plainId = cmd->id;
            if (plainId < 1) {
                normal = 0;
            } else if (plainId > 0x7fff) {
                normal = 0;
            } else {
                normal = 1;
            }
            if (normal) {
                Src021c5f00 src;
                src.w0 = cmd->id;
                src.w2 = cmd->field6;
                src.b4 = 0;
                src.b5 = 0;
                EnqueueEventTag63_021c5f00(&src);
            }
        }
        return;
    }

    ctx3 = (Ctx021c530c*)func_ov017_0218b5b0();
    list3 = ctx3->list6fc;
    fieldObj = ctx3->obj734;
    found = 0;
    if (ListContainsId((ListHead02046b60*)list3, 4)) {
        if (GetFieldA_021bb9f4(fieldObj) == 5 && cmd->id == GetField14e_021bb9fc(fieldObj) + 0x8005) {
            SetFieldA_021bb9ec(fieldObj, 6);
            func_ov017_021c5b44(cmd->field8, cmd->id, GetField14e_021bb9fc(fieldObj), cmd->fieldc, cmd->field10);
            found = 1;
        }
    } else {
        combatant = GetCombatantWithFlag0x100(gs, cmd->field8);
        zoneInfo = (Zone021c530c*)func_02012fe4();
        if (index != 0) {
            return;
        }
        if (combatant != NULL) {
            if (IsField0Null((void**)list3) && combatant->obj3D_.GetField06() == GetZoneId(zoneInfo)) {
                func_ov017_021c5b44(cmd->field8, cmd->id, GetField14e_021bb9fc(fieldObj), cmd->fieldc, cmd->field10);
                found = 1;
            } else if (combatant->obj3D_.GetField06() == GetZoneId(zoneInfo)) {
                void* nodeB14 = ctx3->nodeB14;
                void* nodeB90 = ctx3->nodeB90;
                inB14 = ListContainsNode((ListHead02046b38*)list3, (ListNode02046b38*)nodeB14);
                inB90 = ListContainsNode((ListHead02046b38*)list3, (ListNode02046b38*)nodeB90);
                if (!(inB14 | inB90)) {
                    pending = &ctx3->pending;
                    pending->id = cmd->id;
                    pending->value = cmd->fielde;
                }
            }
        }
    }
    Src021c5f00 src;
    src.w0 = cmd->id;
    src.w2 = cmd->field6;
    src.b4 = found;
    src.b5 = 1;
    EnqueueEventTag63_021c5f00(&src);
}
