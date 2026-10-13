#include <globaldefs.h>
#include <GameState/GameState.h>

struct PartyTransition { unsigned char field0; unsigned char complete; char pad[6]; unsigned char stage; unsigned char member; unsigned char pending; char pad1; unsigned short elapsed; };
struct SearchStruct { char pad[0xfc0]; int active; };
struct SearchStruct0202c1a4;
struct PeerState { char pad[6]; signed char member; unsigned char pending; unsigned char active; signed char result; };
struct GamePeers { char pad[0x74c0]; PeerState peers[3]; };
struct SearchEntry { short field0; unsigned short unused : 1; unsigned short members : 4; unsigned short rest : 11; };
extern "C" SearchStruct* func_0202ae18();
int GetSearchStructCurrentArrEntry(SearchStruct0202c1a4*);
int TestBitBySignedByteIndex(SearchStruct*, int);
int GetField0x3acValue(GameState*);
void EnqueueEventTag173_021d1810(unsigned short, unsigned short, unsigned char);
extern "C" void func_ov017_021d1118(int, int, int, int);
extern "C" void func_ov017_021d1014(int, int, int);
extern "C" void func_ov017_021d0b30(int, int, int);
void SetSearchFlagBit0202c660(SearchStruct*, int);
int IsFieldMinusOne(int*);
extern "C" int func_ov017_02195658(void*);
int TestMaskBitBySignedByteIndex(SearchStruct*, int);
void* GetEntryBySignedByteIndex(SearchStruct*, int);
extern "C" void func_ov017_021c37a4();

// USA: func_ov017_021b11d8
extern "C" ARM void func_ov017_021b11d8(PartyTransition* transition) {
    GameState* game = GameState::GetInstance();
    void* battle = func_ov017_0218b5b0();
    SearchStruct* search = func_0202ae18();
    if (transition->stage == 0) {
        transition->member = GetSearchStructCurrentArrEntry((SearchStruct0202c1a4*)search);
        transition->pending = 0;
        for (int i = 0; i < 4; i++) {
            if (i != transition->member && TestBitBySignedByteIndex(search, i)) transition->pending |= 1 << i;
        }
        EnqueueEventTag173_021d1810(2, -1, (signed char)GetField0x3acValue(game));
        func_ov017_021d1118(-1, 1, 1, 2);
        func_ov017_021d1014(-1, 1, 0);
        transition->stage++;
    } else if (transition->stage == 1) {
        GamePeers* peers = (GamePeers*)GameState::GetInstance();
        PeerState* peer = peers->peers;
        for (int i = 0; i < 3; i++, peer++) {
            if (peer->member >= 4) {
                if (peer->pending || peer->active) return;
                if (peer->result >= 0) return;
            }
        }
        transition->stage++;
    } else if (transition->stage == 2) {
        func_ov017_021d0b30(9, 0, 0);
        SetSearchFlagBit0202c660(search, 0);
        transition->stage++;
    } else if (transition->stage == 3) {
        if (!IsFieldMinusOne((int*)search)) {
            int removed = func_ov017_02195658(battle);
            if (removed > 0) transition->pending &= ~(1 << removed);
            if (!search->active) {
                for (int i = 1; i < 4; i++) {
                    if ((transition->pending & (1 << i)) && !TestMaskBitBySignedByteIndex(search, i)) transition->pending &= ~(1 << i);
                }
            }
            for (int i = 0; i < 4; i++) {
                if (transition->pending & (1 << i)) {
                    SearchEntry* entry = (SearchEntry*)GetEntryBySignedByteIndex(search, i);
                    if (entry && (entry->members & (1 << transition->member))) return;
                }
            }
            if (transition->elapsed < 1000) {
                if (!search->active) transition->elapsed += game->GetEffectiveDeltaTime();
                return;
            }
        }
        func_ov017_021c37a4();
        transition->complete = 1;
    }
}
