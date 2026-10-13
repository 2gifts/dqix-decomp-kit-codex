#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Filesystem/BackgroundLoader.h"

struct AppearanceData {
    short equipment[10];
    unsigned char female : 1;
    unsigned char flags : 7;
    unsigned char variant : 4;
    unsigned char other : 4;
    short fallback;
};
struct AppearanceSource {
    unsigned char padding0[0x488];
    AppearanceData appearance;
    unsigned char padding4a0[0xc8];
    short classId;
    unsigned char modelVariant;
};
struct EquipmentPacked { int field0; unsigned int low : 12; unsigned int model : 8; unsigned int high : 12; };
struct StructDE234_020de234 {
    EquipmentPacked* packed;
    unsigned char padding4[0xc];
    unsigned int first : 10;
    unsigned int second : 10;
    unsigned int category : 8;
    unsigned int high : 4;
    unsigned char padding14[4];
    short id;
};
struct Container020dedd0;
struct EquipmentPreview {
    Object3D objects[10];
    SafeAllocator allocators[10];
    SafeAllocator animationAllocator;
    unsigned char padding794[0x460];
    short tasks[12];
    Container020dedd0* equipment;
    unsigned char paddingc10[2];
    unsigned char loading;
    unsigned char changed;
    unsigned char fieldc14;
    unsigned char classId;
    unsigned char paddingc16[2];
    AppearanceSource* source;
    short rotation;
    unsigned char reset;
};
extern int data_ov023_021fd698[5];
extern char data_ov023_021fd678[8];
extern signed char data_ov023_021fd680[10];
extern char data_ov023_021fdd16[];
extern char data_ov023_021fdd1c[];
extern char data_ov023_021fdd2a[];
extern char data_ov023_021fdd45[];
extern char data_ov023_021fdd58[];
extern "C" void func_ov023_021e61a0(void*);
int GetField0x3acValue(GameState*);
void CopyAndClampShorts_021e5fdc(void*, short*, short*);
StructDE234_020de234* FindElementByKey020dedd0(Container020dedd0*, int);
unsigned short GetPreferredPackedField020de234(StructDE234_020de234*, int);

// USA: func_ov023_021e5974
extern "C" ARM void func_ov023_021e5974(EquipmentPreview* state, AppearanceSource* source, int classId, int unused, int force) {
    if (!source) return;
    state->source = source;
    func_ov023_021e61a0(state);
    GameState* game = GameState::GetInstance();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    AppearanceData* appearance = &source->appearance;
    if (classId < 0) classId = source->classId;
    if (classId < 0) classId = GetField0x3acValue(game);
    short ids[10];
    state->classId = classId;
    CopyAndClampShorts_021e5fdc((void*)classId, ids, appearance->equipment);
    state->objects[3].unknown_2_ = -1;
    state->objects[4].unknown_2_ = -1;
    FindElementByKey020dedd0(state->equipment, ids[7]);
    StructDE234_020de234* items[10] = {};
    items[0] = FindElementByKey020dedd0(state->equipment, ids[0]);
    items[7] = FindElementByKey020dedd0(state->equipment, ids[7]);
    state->rotation = state->objects[0].rotation_.y;
    state->changed = 0;
    for (int i = 0; i < 5; i++) {
        int slot = data_ov023_021fd698[i];
        int previous = state->objects[slot].unknown_2_;
        if (previous != ids[slot] || force) {
            state->changed = 1;
            state->objects[0].unknown_2_ = -1;
            state->objects[6].unknown_2_ = -1;
            state->objects[5].unknown_2_ = -1;
            state->objects[1].unknown_2_ = -1;
            break;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (ids[i] < 0) {
            state->objects[i].Destroy();
            state->objects[i].unknown_2_ = -1;
            if (i == 0) ids[5] = -1;
            continue;
        }
        StructDE234_020de234* item = FindElementByKey020dedd0(state->equipment, ids[i]);
        if (!item) {
            state->objects[i].Initialize();
            state->objects[i].unknown_2_ = ids[i];
            continue;
        }
        state->objects[i].Destroy();
        state->allocators[i].Reset();
        char path[128];
        int pathReady;
        {
            if (!source) { pathReady = 0; goto equipmentPathReady; }
            int female = source->appearance.female;
            StructDE234_020de234* base = items[0];
            unsigned int model;
            StructDE234_020de234* accessory = items[7];
            model = GetPreferredPackedField020de234(item, female);
            char suffix[8] = "nsbmd";
            char variant[2] = {};
            if (i == 4) {
                strcpy(suffix, data_ov023_021fdd16);
                variant[0] = 'a';
                variant[1] = 0;
                model += appearance->variant;
            }
            if (i == 3) {
                variant[0] = 'a';
                variant[1] = 0;
                if (accessory && accessory->id > -1) {
                unsigned int category = GetPreferredPackedField020de234(accessory, female) / 100u;
                if (category < 10) {
                    int letter = data_ov023_021fd680[category];
                    if (!letter) { pathReady = 0; goto equipmentPathReady; }
                    variant[0] = letter;
                    if (category == 3 && female == 0 && appearance->equipment[3] == 0x2329)
                    variant[0] = 'f';
                } else { pathReady = 0; goto equipmentPathReady; }
                }
            }
            if (i == 5) {
                if (appearance->equipment[4] == 0x1f4a || appearance->equipment[4] < 0)
                model = GetPreferredPackedField020de234(base, female);
            }
            if (item->id == 1000) model += source->modelVariant;
            sprintf(path, data_ov023_021fdd1c, (signed char)item->category, model, variant, suffix);
            pathReady = 1;
        }
    equipmentPathReady:
        if (pathReady) {
            state->tasks[i] = loader->QueueLoadFileInGP2(data_ov023_021fdd2a, path, 0);
            state->objects[i].unknown_2_ = ids[i];
        }
    }
    state->changed = 1;
    if (state->changed) {
        StructDE234_020de234* base = FindElementByKey020dedd0(state->equipment, ids[0]);
        StructDE234_020de234* accessory = FindElementByKey020dedd0(state->equipment, ids[8]);
        unsigned char accessoryModel = 0;
        if (accessory) accessoryModel = accessory->packed->model;
        if (base) {
            char sex = 'm';
            if (appearance->female == 1) sex = 'w';
            char path[128];
            sprintf(path, data_ov023_021fdd45, base->packed->model, accessoryModel, sex);
            state->tasks[10] = loader->QueueLoadFileInGP2(data_ov023_021fdd2a, path, 0);
            sprintf(path, data_ov023_021fdd58, base->packed->model, accessoryModel, sex);
            state->tasks[11] = loader->QueueLoadFileInGP2(data_ov023_021fdd2a, path, 0);
            if (state->objects[0].pModel_) state->objects[0].pModel_->RemoveAnimations();
            state->objects[0].RemoveAllAnimationPackages();
            state->animationAllocator.Reset();
        }
    }
    state->loading = 1;
    state->reset = 0;
}
