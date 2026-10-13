#include <globaldefs.h>
#include "Combat/ActionDisplay.h"
#include "GameState/GameState.h"
#include "System/ColorEffects.h"
#include "System/Memory.h"
#include "std_library_functions.h"

struct Bcb8Params;
struct Entry_02027878 {
    char padding0[8];
    Bcb8Params* forwarding;
};
struct List_02027878;
struct BattleTimer0202441c;
struct Actor02024d48;
struct ForwardingRecord0202555c {
    int field_0x0;
    int entryIndex;
    int x;
    int y;
};
struct Slot0202555c {
    char data[0x28];
};
struct Receiver0202555c {
    char padding0[0x18];
    int forwardingX;
    int forwardingY;
    char padding20[0xc];
    ForwardingRecord0202555c* forwarding;
    int forwardingCount;
    char padding34[4];
    fix32_t scale;
    int tileX;
    int tileY;
    int screenX;
    int screenY;
    char padding4c[0x74 - 0x4c];
    int forwardingFill;
    char padding78[0x94 - 0x78];
    int bits94;
    char padding98[0xe8 - 0x98];
    Slot0202555c slots[1];
    char padding110[0x55c - 0x110];
    unsigned char forwardingPending;
    char padding55d[0x5b0 - 0x55d];
    fix32_t cameraHeight;
    char padding5b4[0x9c0 - 0x5b4];
    unsigned char actionState;
};
void* GetData02105254();
extern "C" void* func_02012fe4();
void SetArraySlotFlag0203b718(void*, int, int, int);
Entry_02027878* GetEntryFromList(List_02027878*, int);
void ForwardParamsToB8bc(Bcb8Params*, int, int);
void AccumulateBattleTimer0202441c(BattleTimer0202441c*);
int CheckCombatantReadyForAction020243b8(void*, int);
int CheckActionAllowed(Actor02024d48*, int);
void SetElementFields0202756c(void*, int, int, int, unsigned char, unsigned char, unsigned short, unsigned char, int, int);
void SetBitfield0205afdc(int, void*, int);

// USA: func_0202555c
extern "C" ARM void func_0202555c(Receiver0202555c* self) {
    GameObject* object = GameState::GetInstance()->GetUnknownGameObject();
    void* displayState = GetData02105254();
    func_02012fe4();
    Vector3i position = object->obj3D_.position_;
    CombatActionPosition entries[17];
    unsigned char groups[32];
    memset(groups, 0, sizeof(groups));
    int count = func_020236dc(entries, groups, &position);
    fix32_t scale = self->scale;
    int screenX = (int)((float)FIX32_MULTIPLY(position.x, scale) / 4096.0f);
    fix32_t oldHeight = self->cameraHeight;
    int cameraY = (int)((float)FIX32_MULTIPLY(self->cameraHeight, scale) / 4096.0f);
    int objectY = (int)((float)FIX32_MULTIPLY(position.z, scale) / 4096.0f);
    int below = cameraY - objectY;
    int above = objectY - cameraY;
    if (above > 0x1e) {
        self->cameraHeight += fix32_Divide((above - 0x1e) << 12, scale);
    } else if (below > 0x37) {
        self->cameraHeight -= fix32_Divide((below - 0x37) << 12, scale);
    }
    fix32_t product = FIX32_MULTIPLY(oldHeight, self->cameraHeight);
    if (product < 0) {
        self->cameraHeight = 0;
    }
    scale = self->scale;
    cameraY = (int)((float)FIX32_MULTIPLY(self->cameraHeight, scale) / 4096.0f);
    int tileX = screenX < 0 ? screenX / 8 - 1 : screenX / 8;
    int tileY = cameraY < 0 ? cameraY / 8 - 1 : cameraY / 8;
    if (self->forwardingY - tileY + 14 != 0 || cameraY % 8 != 0) {
        if (above > 0) {
            self->cameraHeight = fix32_Divide((objectY - 0x1e) << 12, scale);
        } else if (below > 0) {
            self->cameraHeight = fix32_Divide((objectY + 0x37) << 12, scale);
        }
    }
    cameraY = (int)((float)FIX32_MULTIPLY(self->cameraHeight, self->scale) / 4096.0f);
    tileY = cameraY < 0 ? cameraY / 8 - 1 : cameraY / 8;
    self->screenX = screenX;
    self->screenY = cameraY;
    if (self->tileY != tileY) {
        self->forwardingPending = 1;
    }
    self->tileX = tileX;
    self->tileY = tileY;
    if (self->forwardingPending) {
        self->forwardingPending = 0;
        SetArraySlotFlag0203b718(displayState, 1, 0, self->forwardingFill);
        for (int i = 0; i < self->forwardingCount; ++i) {
            Entry_02027878* entry = GetEntryFromList((List_02027878*)self, self->forwarding[i].entryIndex);
            if (entry) {
                ForwardParamsToB8bc((Bcb8Params*)((char*)entry + 8), self->forwarding[i].x,
                                    self->forwardingY + self->forwarding[i].y - tileY + 16);
            }
        }
    }
    AccumulateBattleTimer0202441c((BattleTimer0202441c*)self);
    for (int i = 0; i < count; ++i) {
        CombatActionPosition* entry = &entries[i];
        if (!CheckCombatantReadyForAction020243b8(self, entry->combatantId)) {
            continue;
        }
        scale = self->scale;
        int x = (int)((float)FIX32_MULTIPLY(entry->position.x, scale) / 4096.0f) - (self->forwardingX << 3);
        int y = (int)((float)FIX32_MULTIPLY(entry->position.z, scale) / 4096.0f) - self->screenY;
        if (entry->variant >= 0) {
            int group = groups[entry->group] - 1;
            x += data_020e70f0[group][entry->variant][0];
            y += data_020e70f4[group][entry->variant][0];
        }
        int isSpecial = entry->type == 0x74;
        if (isSpecial) {
            SetElementFields0202756c(self, (x - 4) << 12, (y + 0x5c) << 12, entry->elementId, entry->type, 0,
                                     entry->palette, 0xff, 0x1000, 0x1000);
            continue;
        }
        if (!CheckActionAllowed((Actor02024d48*)entry, self->actionState)) {
            continue;
        }
        int slot = (short)entry->elementId * (short)40;
        if (entry->flag13) {
            ColorEffect_ConfigureAlphaBlend(0x04001050, 0, 3, 8, 8);
            SetBitfield0205afdc((int)&self->bits94, (char*)self->slots + slot, 1);
        } else {
            SetBitfield0205afdc((int)&self->bits94, (char*)self->slots + slot, 0);
        }
        SetElementFields0202756c(self, (x - 4) << 12, (y + 0x5c) << 12, entry->elementId, entry->type, 0,
                                 entry->palette, 0xff, 0x1000, 0x1000);
    }
    func_02023b5c(self, self->forwardingX << 3, self->screenY, -4, 0x5c, 0);
    func_02023d84(self, entries, count, self->forwardingX << 3, self->screenY, -4, 0x5c, 0);
}
