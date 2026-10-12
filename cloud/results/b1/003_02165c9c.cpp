#include <globaldefs.h>
#include "GameState/GameState.h"
#include "std_library_functions.h"

struct Container020e0310 {
    unsigned char data[0x48];
};
struct Outer02166164;
struct Struct020865b0;
struct S0215f56c;

struct EntryList02165c9c {
    unsigned char pad0[0x36];
    short activeId;
};

struct PartyMember02165c9c {
    signed char id : 6;
    signed char idRest : 2;
    unsigned char pad1;
    unsigned char vocations[13];
    unsigned char ranks[13];
    unsigned char pad1c[0x140 - 0x1c];
    unsigned char name[0x34];
    unsigned char guest : 1;
    unsigned char guestRest : 7;
    unsigned char pad175[0x23c - 0x175];
};

struct PartyData02165c9c {
    unsigned char pad0[0xf7c];
    unsigned char leaderOffset;
    unsigned char pad0f7d[3];
    PartyMember02165c9c members[13];
    unsigned char count;
};

struct NameBuffer02165c9c {
    char text[0x30];
};

struct BattleMenu02165c9c {
    unsigned char pad0[0x2dc];
    Container020e0310 messages;
    EntryList02165c9c* list;
    unsigned char pad328[0x39c - 0x328];
    NameBuffer02165c9c names[4];
    unsigned char pad45c[0x468 - 0x45c];
    unsigned char slots[4];
    unsigned char pad46c[0x480 - 0x46c];
    short selectedId;
    unsigned char pad482[0x49f - 0x482];
    signed char page;
    unsigned char pad4a0[0x4bd - 0x4a0];
    unsigned char firstId;
};

struct ShortPair02165c9c {
    short ids[2];
};

struct DataTable0217f3ec {
    unsigned char pad0[0x10];
    ShortPair02165c9c entryIds;
};

extern DataTable0217f3ec data_ov003_0217f3ec;

void SetSublistEntriesFlag1(void*, int);
const char* GetFieldByKey020e0434(Container020e0310*, int);
int CompareThreshold_021614e8(int);
void SetOrClearFlags464_02166164(Outer02166164*, int, int);
extern "C" void func_02042764(unsigned char*, char*, int);
extern "C" void _Z26SetEntryFirstField02080f8cP17Container02080f8cii(EntryList02165c9c*, int, const char*);
int GetFieldAt0x50(Struct020865b0*);
void ResolveEntryValueByKey02080fe0(void*, int, int);
extern "C" void _Z26SetEntryFirstField02080fa8P17Container02080fa8ii(EntryList02165c9c*, int, int);
int ClassifyRatio_0215f56c(S0215f56c*);
extern "C" void _Z22SetEntryHighNibble0x13P17Container02080cc0ii(EntryList02165c9c*, short, int);
extern "C" void _Z24SetEntryFlagById02080b40Pvi(void*, short);
extern "C" void _Z19SetEntryHalfword0xePvii(void*, short, short);
extern "C" void _Z27GetEntryFieldsAt0x602080828PviPsS0_(void*, short, short*, short*);
extern "C" void _Z23GetEntryOutputs020808c8PviPsS0_(void*, short, short*, short*);
void SetEntryFieldsAt0x602080854(void*, int, int, int);
extern "C" void func_ov003_0215f5e8(void*, int, int);
extern "C" void func_ov003_02160c58(BattleMenu02165c9c*, short*, short*);
extern "C" void _Z23SetEntryNibbleByShortIdP19StatusOwner02081198ii(EntryList02165c9c*, int, int);
extern "C" void _Z24SetEntryFlag0x2ByShortIdP19StatusOwner02081164ii(EntryList02165c9c*, int, int);
extern "C" void func_020813ec(void*, int);
extern "C" void func_ov003_02160678(BattleMenu02165c9c*);
extern "C" void func_ov003_02160794(BattleMenu02165c9c*);

// USA: func_ov003_02165c9c
extern "C" ARM void func_ov003_02165c9c(BattleMenu02165c9c* self) {
    EntryList02165c9c* list = self->list;
    SetSublistEntriesFlag1(list, 6);
    PartyData02165c9c* party = (PartyData02165c9c*)GetPtrField0x2a04(GameState::GetInstance());
    PartyMember02165c9c* m;
    short id = 0x15;
    short rankId = 0xcb;
    int count = party->count;
    short iconId;
    short member = (short)(self->page * 4);
    iconId = 0x64;
    short vocation = -1;
    const char* msgA = GetFieldByKey020e0434(&self->messages, 0x7530);
    const char* msgB = GetFieldByKey020e0434(&self->messages, 0x7531);
    self->firstId = id;
    memset(self->slots, 0, 4);
    int pending = CompareThreshold_021614e8((int)self) == 0 ? 1 : 0;
    int total = count + pending;
    SetOrClearFlags464_02166164((Outer02166164*)self, total > 4, 1);
    memset(self->names, 0, sizeof(self->names));

    for (short i = 0; i < 4; i++) {
        m = &party->members[member];
        func_02042764(m->name, self->names[i].text, 1);
        _Z26SetEntryFirstField02080f8cP17Container02080f8cii(list, id, self->names[i].text);
        if (GetFieldAt0x50((Struct020865b0*)m) != 0) {
            vocation = (short)(GetFieldAt0x50((Struct020865b0*)m) + 0x11);
        }
        ResolveEntryValueByKey02080fe0(list, (short)(id + 4), vocation);
        _Z26SetEntryFirstField02080fa8P17Container02080fa8ii(list, (short)(id + 8),
            m->vocations[GetFieldAt0x50((Struct020865b0*)m)]);
        int shade = ClassifyRatio_0215f56c((S0215f56c*)m);
        if (id == list->activeId && shade == 0xf) {
            shade = 0xa;
        }
        if (member < count) {
            if (m->id == self->selectedId) {
                shade = 5;
            }
            for (short j = 0; j < 3; j++) {
                int entry = id + j * 4;
                _Z22SetEntryHighNibble0x13P17Container02080cc0ii(list, entry, shade);
                _Z24SetEntryFlagById02080b40Pvi(list, entry);
            }
            _Z22SetEntryHighNibble0x13P17Container02080cc0ii(list, iconId, shade);
            _Z24SetEntryFlagById02080b40Pvi(list, iconId);
            unsigned char rank = m->ranks[GetFieldAt0x50((Struct020865b0*)m)];
            _Z19SetEntryHalfword0xePvii(list, rankId, rank + 0x64);
            if (rank != 0) {
                shade = 5;
                if (rank == 10) {
                    shade = 0xd;
                }
                _Z22SetEntryHighNibble0x13P17Container02080cc0ii(list, rankId, shade);
                int row = i + 0x19;
                short x, y, w, h;
                _Z27GetEntryFieldsAt0x602080828PviPsS0_(list, row, &x, &y);
                _Z23GetEntryOutputs020808c8PviPsS0_(list, row, &w, &h);
                SetEntryFieldsAt0x602080854(list, rankId, (short)(x + w + 1), y);
                _Z22SetEntryHighNibble0x13P17Container02080cc0ii(list, rankId, shade);
                _Z24SetEntryFlagById02080b40Pvi(list, rankId);
            }
            self->slots[i] = m->guest + 1;
        } else {
            const char* msg = msgA;
            if (pending) {
                if (CompareThreshold_021614e8((int)self) == 0) {
                    pending = 0;
                    msg = msgB;
                }
            }
            _Z22SetEntryHighNibble0x13P17Container02080cc0ii(list, id, shade);
            _Z24SetEntryFlagById02080b40Pvi(list, id);
            _Z26SetEntryFirstField02080f8cP17Container02080f8cii(list, id, msg);
        }
        member++;
        id++;
        rankId++;
        iconId++;
    }

    func_ov003_0215f5e8(list, self->page, total);
    short high, low;
    func_ov003_02160c58(self, &high, &low);
    low = low - (((PartyData02165c9c*)GetPtrField0x2a04(GameState::GetInstance()))->leaderOffset - 1);
    _Z26SetEntryFirstField02080fa8P17Container02080fa8ii(list, 0x23, low);
    _Z26SetEntryFirstField02080fa8P17Container02080fa8ii(list, 0x24, high);

    ShortPair02165c9c ids = data_ov003_0217f3ec.entryIds;
    for (int k = 0; k < 2; k++) {
        short entryId = ids.ids[k];
        _Z23SetEntryNibbleByShortIdP19StatusOwner02081198ii(list, entryId, 1);
        _Z24SetEntryFlag0x2ByShortIdP19StatusOwner02081164ii(list, entryId, 1);
        func_020813ec(list, entryId);
    }
    func_ov003_02160678(self);
    func_ov003_02160794(self);
}
