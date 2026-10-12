#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Model3D.h"
#include "Graphics/VRAMStaging.h"
#include "Memory/AllocatorUnion.h"
#include "System/Cache.h"

GameObject* GetField4334_021bdbc0(unsigned char* base);
void* AllocateAligned4(AllocatorUnion* alloc, unsigned int size);
void TailForward02012da4(AllocatorUnion* alloc, void* data);

struct SlotIdTable_0218fd34 {
    int v[14];
};
extern struct SlotIdTable_0218fd34 data_ov017_021d6260;
extern AllocatorUnion data_02114e20;

static inline unsigned short GetColorR(unsigned short c) {
    return c & 0x1f;
}

static inline unsigned short GetColorG(unsigned short c) {
    return (c & 0x3e0) >> 5;
}

static inline unsigned short GetColorB(unsigned short c) {
    return (c & 0x7c00) >> 10;
}

static inline unsigned int GetPaletteVRAMOffset(NSBXXTex* tex) {
    return (tex->block4VRAMLoadOffset_ & 0xffff) << 3;
}

// USA: func_ov017_0218fd34
extern "C" ARM void func_ov017_0218fd34(unsigned char* ov, int idx, unsigned short color) {
    GameState* battleStruct = GameState::GetInstance();

    struct SlotIdTable_0218fd34 table = data_ov017_021d6260;
    int base = idx * 12;
    table.v[0] = idx;
    table.v[1] = base + 0x13;
    table.v[2] = base + 0x14;
    table.v[3] = base + 0x16;
    table.v[4] = base + 0x17;
    table.v[5] = base + 0x18;
    table.v[6] = base + 0x19;
    table.v[7] = base + 0x1a;
    table.v[8] = base + 0x1b;
    table.v[9] = base + 0x1c;
    table.v[10] = base + 0x1d;

    unsigned short addR = GetColorR(color);
    unsigned short addG = GetColorG(color);
    unsigned short addB = GetColorB(color);

    unsigned short* bufs[13] = {0};

    int mode = 0;
    GameObject* first = battleStruct->GetGameObjectByIndex(table.v[0]);
    if (first != NULL) {
        mode = first->obj3D_.GetField9e();
    }
    LockStagedTextureVRAMCopying();

    int i;
    int id;
    for (i = 0; (id = table.v[i]) > -1; i++) {
        GameObject* obj;
        if (id == 0xff) {
            obj = GetField4334_021bdbc0(ov);
        } else {
            obj = battleStruct->GetGameObjectByIndex(id);
        }
        if (obj == NULL) {
            continue;
        }
        if (obj->obj3D_.unknown_2_ <= -1 && id != 0xff) {
            continue;
        }
        Model3D* model = obj->obj3D_.pModel_;
        if (model == NULL) {
            continue;
        }
        NSBXXTex* tex = model->GetTEX0();
        if (tex == NULL) {
            continue;
        }
        unsigned short* src;
        unsigned int size;
        unsigned int offset;
        offset = GetPaletteVRAMOffset(tex);
        src = (unsigned short*)((char*)tex + tex->block4Offset_);
        size = tex->block4NumEightBytes_ << 3;

        if (mode == 0) {
            bufs[i] = (unsigned short*)AllocateAligned4(&data_02114e20, size);
            if (bufs[i] == NULL) {
                continue;
            }
            for (unsigned int j = 0; j < size / 2; j++) {
                unsigned short c = src[j];
                unsigned short r = GetColorR(c);
                unsigned short g = GetColorG(c);
                unsigned short b = GetColorB(c);
                r += addR;
                g += addG;
                b += addB;
                if (r > 31) {
                    r = 31;
                }
                if (g > 31) {
                    g = 31;
                }
                if (b > 31) {
                    b = 31;
                }
                bufs[i][j] = r | (g << 5) | (b << 10);
            }
            CleanInvalidateCacheRange(bufs[i], size);
            StageMemoryToVRAM(VRAMSubregion_TexturePalette, bufs[i], offset, size, false, true);
        } else if (mode == 1) {
            bufs[i] = (unsigned short*)AllocateAligned4(&data_02114e20, size);
            if (bufs[i] == NULL) {
                continue;
            }
            for (unsigned int j = 0; j < size / 2; j++) {
                bufs[i][j] = color;
            }
            CleanInvalidateCacheRange(bufs[i], size);
            StageMemoryToVRAM(VRAMSubregion_TexturePalette, bufs[i], offset, size, false, true);
        } else if (mode == 2) {
            bufs[i] = (unsigned short*)AllocateAligned4(&data_02114e20, size);
            if (bufs[i] == NULL) {
                continue;
            }
            float scaleR = addR / 31.0f;
            float scaleG = addG / 31.0f;
            float scaleB = addB / 31.0f;
            for (unsigned int j = 0; j < size / 2; j++) {
                unsigned short c = src[j];
                unsigned short r = GetColorR(c);
                unsigned short g = GetColorG(c);
                unsigned short b = GetColorB(c);
                unsigned short nr = r * scaleR;
                unsigned short ng = g * scaleG;
                unsigned short nb = b * scaleB;
                bufs[i][j] = nr | (ng << 5) | (nb << 10);
            }
            CleanInvalidateCacheRange(bufs[i], size);
            StageMemoryToVRAM(VRAMSubregion_TexturePalette, bufs[i], offset, size, false, true);
        }
    }
    UnlockStagedTextureVRAMCopying();

    for (unsigned int k = 0; k < 13; k++) {
        if (bufs[k] != NULL) {
            TailForward02012da4(&data_02114e20, bufs[k]);
        }
    }
}
