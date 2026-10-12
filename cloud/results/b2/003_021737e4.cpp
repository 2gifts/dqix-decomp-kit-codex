#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Resource/GameResources.h"
#include "System/BGBases.h"
#include "System/Cache.h"
#include "System/LoadToVRAM.h"
#include "System/Memory.h"

struct ActiveEntry02046900;
struct Rec020467f0;
struct TableA68;

extern "C" void func_020a620c(void* obj);
extern "C" void func_0204c684(void* obj);
extern "C" void* func_02012fe4();
extern "C" void __clear(void* dst, int count);
extern "C" void func_ov003_02172618(void* canvas, int y, char** labels);
int CountActiveEntries(struct ActiveEntry02046900* entry);
void* FindRecordByIndex(struct Rec020467f0* rec, int index, void** out, int* outSize);
void* FindEntryByKey(struct TableA68* table, int key);

extern char data_ov003_02180a76[];

struct Canvas021737e4 {
    char pad0[8];
    void* field8;
    char padc[0xa8 - 0xc];
    unsigned short halfA8;
    unsigned short halfAA;
    char padAC[0xb4 - 0xac];
    unsigned short halfB4;
    unsigned short halfB6;
    char padB8[0xe0 - 0xb8];
};

struct Obj021737e4 {
    char pad0;
    unsigned char mode;
    short kind;
    char labelA[0x40];
    char labelB[0x80];
    char padc4[2];
    short scrollY;
    short targetY;
    char padca[0xd4 - 0xca];
    unsigned char bgIndex;
    char padd5[3];
    unsigned char fade[0x15];
    unsigned char modeEd;
    char padee;
    unsigned char state;
    char padf0[4];
    int handle;
    char padf8[0x1d8 - 0xf8];
    char table[1];
};

// USA: func_ov003_021737e4
extern "C" ARM void func_ov003_021737e4(struct Obj021737e4* self) {
    func_ov017_0218b5b0();
    BackgroundLoader* loader = BackgroundLoader::GetInstance();
    func_020a620c(self->fade);

    if (self->state == 0) {
        self->handle = loader->QueueLoadFile(data_ov003_02180a76, 0);
        self->state = 1;
    } else if (self->state == 1) {
        if (loader->GetTaskStatus(self->handle) != 0) {
            void* recData;
            void* file;
            unsigned int length;
            unsigned char* recs[3];
            int sizes[3];
            loader->GetLoadedFileByID(self->handle, &file, &length);
            int count = CountActiveEntries((struct ActiveEntry02046900*)file);
            for (int i = 0; i < count; i++) {
                recs[i] = (unsigned char*)FindRecordByIndex((struct Rec020467f0*)file, i, &recData, &sizes[i]);
            }
            unsigned char* rec = self->mode != 0 ? recs[1] : recs[0];
            Canvas021737e4 canvas;
            func_0204c684(&canvas);
            canvas.field8 = rec + 0x10;
            canvas.halfA8 = 0x20;
            canvas.halfAA = 8;
            canvas.halfB4 = 0xc;
            canvas.halfB6 = 0xc;
            func_02012fe4();
            if (self->mode != 0) {
                char* labels[2];
                __clear(labels, 8);
                labels[1] = self->labelA;
                int y = 4;
                if (self->kind == 1) {
                    labels[0] = (char*)FindEntryByKey((struct TableA68*)self->table, 0xc8);
                    y = 0xf;
                } else if (self->kind == 2) {
                    labels[0] = (char*)FindEntryByKey((struct TableA68*)self->table, 0xc9);
                    y = 0xf;
                }
                func_ov003_02172618(&canvas, y, labels);
            } else {
                char* labels[2] = { self->labelB, self->labelA };
                func_ov003_02172618(&canvas, 0xf, labels);
            }
            int offset = (self->bgIndex - 1) << 13;
            CleanInvalidateCacheRange(rec + 0x10, 0x2000);
            LoadToSubBG0CharacterData(rec + 0x10, offset + 0x1000, 0x2000);
            int palette = 2;
            if (self->mode != 0) {
                palette = 3;
            }
            unsigned short (*map)[0x100] = (unsigned short (*)[0x100])GetSubBG0ScreenBase();
            for (int i = 0; i < 0x100; i++) {
                int row = self->bgIndex - 1;
                unsigned int attr = palette << 28;
                (map[0] + i)[row * 0x100] = (unsigned short)(i + 0x80 + (row << 8)) | (attr >> 16);
            }
            self->targetY = (self->bgIndex << 6) - 0xc0;
            loader->RemoveTask(self->handle);
            self->handle = -1;
            self->state = 2;
        }
    } else if (self->state == 2) {
        if (self->scrollY < self->targetY) {
            self->scrollY += (int)((self->targetY - self->scrollY) * 0.4f) + 1;
            *(volatile unsigned int*)0x4001010 = (self->scrollY << 16) & 0x1ff0000;
        }
        if (self->scrollY >= self->targetY) {
            self->scrollY = self->targetY;
            *(volatile unsigned int*)0x4001010 = (self->scrollY << 16) & 0x1ff0000;
            self->mode = 0;
            self->kind = -1;
            VectorizedMemset(self->labelA, 0, 0x40);
            VectorizedMemset(self->labelB, 0, 0x80);
            self->modeEd = 2;
            self->state = 0;
        }
    }
}
