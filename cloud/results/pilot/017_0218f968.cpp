#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Memory/AllocatorUnion.h"
#include "Graphics/VRAMStaging.h"
#include "System/Cache.h"

void* AllocateAligned4(AllocatorUnion* alloc, unsigned int size);
void TailForward02012da4(AllocatorUnion* alloc, void* data);
extern AllocatorUnion data_02114e20;

// USA: func_ov017_0218f968
extern "C" ARM void func_ov017_0218f968(int, int index, unsigned short color) {
    unsigned short* output;
    unsigned short* source;
    unsigned int length;
    unsigned int offset;
    GameState* state = GameState::GetInstance();
    unsigned short red = color & 31;
    unsigned short green = (color & 0x3e0) >> 5;
    unsigned short blue = (color & 0x7c00) >> 10;
    output = NULL;
    LockStagedTextureVRAMCopying();
    GameObject* object = state->GetGameObjectByIndex(index);
    if (object != NULL && object->obj3D_.pModel_ != NULL) {
        NSBXXTex* texture = object->obj3D_.pModel_->GetTEX0();
        if (texture != NULL) {
            if (object->obj3D_.GetField9e() == 0) {
                offset = texture->block4VRAMLoadOffset_;
                if (object->obj3D_.GetTexturePaletteOffset() != 0)
                    offset = object->obj3D_.GetTexturePaletteOffset();
                length = texture->block4NumEightBytes_ << 3;
                offset = (offset & 0xffff) << 3;
                source = (unsigned short*)((char*)texture + texture->block4Offset_);
                output = (unsigned short*)AllocateAligned4(&data_02114e20, length);
                if (output != NULL) {
                    for (unsigned int i = 0; i < length / 2; i++) {
                        unsigned short r = source[i] & 31;
                        unsigned short g = (source[i] & 0x3e0) >> 5;
                        unsigned short b = (source[i] & 0x7c00) >> 10;
                        r += red;
                        g += green;
                        b += blue;
                        if (r > 31) r = 31;
                        if (g > 31) g = 31;
                        if (b > 31) b = 31;
                        output[i] = r | (g << 5) | (b << 10);
                    }
                    CleanInvalidateCacheRange(output, length);
                    StageMemoryToVRAM(VRAMSubregion_TexturePalette, output, offset, length, false, true);
                }
            } else if (object->obj3D_.GetField9e() == 1) {
                offset = texture->block4VRAMLoadOffset_;
                if (object->obj3D_.GetTexturePaletteOffset() != 0)
                    offset = object->obj3D_.GetTexturePaletteOffset();
                length = texture->block4NumEightBytes_ << 3;
                offset = (offset & 0xffff) << 3;
                output = (unsigned short*)AllocateAligned4(&data_02114e20, length);
                if (output != NULL) {
                    for (unsigned int i = 0; i < length / 2; i++) {
                        output[i] = color;
                    }
                    CleanInvalidateCacheRange(output, length);
                    StageMemoryToVRAM(VRAMSubregion_TexturePalette, output, offset, length, false, true);
                }
            } else if (object->obj3D_.GetField9e() == 2) {
                offset = texture->block4VRAMLoadOffset_;
                if (object->obj3D_.GetTexturePaletteOffset() != 0)
                    offset = object->obj3D_.GetTexturePaletteOffset();
                length = texture->block4NumEightBytes_ << 3;
                offset = (offset & 0xffff) << 3;
                source = (unsigned short*)((char*)texture + texture->block4Offset_);
                output = (unsigned short*)AllocateAligned4(&data_02114e20, length);
                if (output != NULL) {
                    float redScale = red / 31.0f;
                    float greenScale = green / 31.0f;
                    float blueScale = blue / 31.0f;
                    for (unsigned int i = 0; i < length / 2; i++) {
                        unsigned short r = source[i] & 31;
                        unsigned short g = (source[i] & 0x3e0) >> 5;
                        unsigned short b = (source[i] & 0x7c00) >> 10;
                        r = (unsigned short)(r * redScale);
                        g = (unsigned short)(g * greenScale);
                        b = (unsigned short)(b * blueScale);
                        output[i] = r | (g << 5) | (b << 10);
                    }
                    CleanInvalidateCacheRange(output, length);
                    StageMemoryToVRAM(VRAMSubregion_TexturePalette, output, offset, length, false, true);
                }
            }
        }
    }
    UnlockStagedTextureVRAMCopying();
    if (output != NULL)
        TailForward02012da4(&data_02114e20, output);
}
