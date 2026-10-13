#include <globaldefs.h>

struct Reader;
struct ReaderVtable {
    void* field_00;
    void* field_04;
    int (*seek)(Reader*, unsigned int);
    int (*read)(Reader*, void*, unsigned int);
};
struct Reader {
    ReaderVtable* vtable;
    unsigned int field_04;
    unsigned int position;
};
struct Pool {
    unsigned char pad_00[4];
    unsigned int width;
    unsigned int height;
    unsigned char pad_0c[0x30];
    void* blockA;
    int* table;
    char* blockC;
    unsigned char pad_48[0x40c];
};
struct Entry {
    unsigned char pad_00[0x1cc];
    int stateA;
    unsigned char pad_1d0[0x6f0];
    unsigned char data[0xc34];
    int stateB;
};
struct Header {
    signed char magic[5];
    signed char version;
    unsigned short format;
    unsigned char pad_08[4];
    unsigned int width;
    unsigned int height;
    unsigned char pad_14[4];
    unsigned short kind;
    unsigned short entryCount;
    unsigned char pad_1c[4];
    unsigned int size;
    unsigned int offsetA;
    unsigned int offsetB;
    unsigned int tableCount;
};
struct Loader {
    Reader* reader;
    Header header;
    Pool* pool;
    void* blockD;
    unsigned char pad_3c[0x10];
    unsigned int width;
    unsigned int height;
    void* blockB;
    Entry* entries;
    void** bufferA;
    void** bufferB;
    int* indexes;
    unsigned char pad_68[8];
    void* work[2];
    unsigned int length;
    unsigned char pad_7c[4];
    unsigned int zero80;
    unsigned char pad_84[4];
    unsigned int mask;
    unsigned char pad_8c[4];
    int loaded;
    void* table;
    int zero98;
    int zero9c;
    int zeroA0;
    unsigned char pad_a4[4];
    unsigned int count;
    int remain[6];
    unsigned int zeroC4;
    unsigned char pad_c8[0xc];
    unsigned int basePosition;
};

void* SafeAllocatorAllocate_0218bc8c(unsigned int);
extern "C" void func_020ca458(int, void*, unsigned int);
void* GetOrStreamPoolBlockD_0218fcd4();
void* GetOrStreamPoolBlockA_0218fd50();
char* GetOrStreamPoolBlockC_0218fe30();
void* GetOrStreamPoolBlockB_0218fdc0();
extern "C" void VectorizedMemset(void*, int, unsigned int);

extern int data_ov016_02191d78[];

// USA: func_ov016_0218e800
extern "C" ARM int func_ov016_0218e800(Loader* loader, Reader* reader, unsigned int count) {
    int ok = 0;
    loader->reader = reader;
    loader->basePosition = reader->position;
    if (!loader->reader->vtable->read(loader->reader, &loader->header, 0x30)) return ok;
    if (!(loader->header.magic[0] == 'M' && loader->header.magic[1] == 'O' && loader->header.magic[2] == 'D' &&
          loader->header.magic[3] == 'S' && loader->header.magic[4] == 'N' &&
          (loader->header.version == '2' || loader->header.version == '3'))) return ok;
    if (loader->header.format != 10) return ok;
    if (loader->header.magic[4] == 'N' && loader->header.version == '3') {
        signed char tag[2];
        unsigned short length;
        do {
            if (!loader->reader->vtable->read(loader->reader, tag, 2)) return ok;
            if (!loader->reader->vtable->read(loader->reader, &length, 2)) return ok;
            if (!loader->reader->vtable->seek(loader->reader, length * 4 + loader->reader->position)) return ok;
        } while (!(tag[0] == 'H' && tag[1] == 'E'));
    }
    if (loader->header.kind != 0 && loader->header.kind != 1 && loader->header.kind != 2 &&
        loader->header.kind != 3 && loader->header.kind != 4) return ok;
    loader->pool = (Pool*)SafeAllocatorAllocate_0218bc8c(0x454);
    func_020ca458(0, loader->pool, 0x454);
    loader->blockD = GetOrStreamPoolBlockD_0218fcd4();
    loader->pool->blockA = GetOrStreamPoolBlockA_0218fd50();
    loader->pool->table = data_ov016_02191d78;
    loader->pool->blockC = GetOrStreamPoolBlockC_0218fe30() + 0x40;
    loader->width = loader->header.width;
    loader->height = loader->header.height;
    loader->blockB = GetOrStreamPoolBlockB_0218fdc0();
    loader->pool->width = loader->header.width;
    loader->pool->height = loader->header.height;
    if (loader->header.width > 0x100) return ok;
    if (count < 6) return ok;
    loader->count = count;
    loader->bufferA = (void**)SafeAllocatorAllocate_0218bc8c(count * 4);
    if (!loader->bufferA) return ok;
    loader->bufferB = (void**)SafeAllocatorAllocate_0218bc8c(loader->count * 4);
    if (!loader->bufferB) return ok;
    for (unsigned int i = 0; i < loader->count; i++) {
        loader->bufferA[i] = SafeAllocatorAllocate_0218bc8c(loader->height << 8);
        if (!loader->bufferA[i]) return ok;
        loader->bufferB[i] = SafeAllocatorAllocate_0218bc8c((loader->height >> 1) << 8);
        if (!loader->bufferB[i]) return ok;
        func_020ca458(0, loader->bufferA[i], loader->height << 8);
        VectorizedMemset(loader->bufferB[i], 0x80, (loader->height >> 1) << 8);
    }
    loader->indexes = (int*)SafeAllocatorAllocate_0218bc8c(loader->count * 4);
    if (!loader->indexes) return ok;
    loader->zero98 = 0;
    loader->remain[0] = 0;
    for (int i = 1; i < 6; i++) loader->remain[i] = loader->count - i;
    loader->zeroC4 = 0;
    loader->zeroA0 = 0;
    loader->zero9c = 0;
    for (int i = 0; i < 2; i++) {
        loader->work[i] = SafeAllocatorAllocate_0218bc8c(loader->header.size + 0x400);
        if (!loader->work[i]) return ok;
    }
    if (loader->header.kind != 0) {
        loader->entries = (Entry*)SafeAllocatorAllocate_0218bc8c(loader->header.entryCount * 0x14f8);
        if (!loader->entries) return ok;
        func_020ca458(0, loader->entries, loader->header.entryCount * 0x14f8);
    }
    unsigned int saved = loader->reader->position;
    loader->reader->vtable->seek(loader->reader, loader->header.offsetB + loader->basePosition);
    loader->table = SafeAllocatorAllocate_0218bc8c(loader->header.tableCount << 3);
    if (!loader->reader->vtable->read(loader->reader, loader->table, loader->header.tableCount << 3)) return ok;
    loader->reader->vtable->seek(loader->reader, saved);
    if (loader->header.kind == 1) {
        unsigned int position = loader->reader->position;
        loader->reader->vtable->seek(loader->reader, loader->header.offsetA + loader->basePosition);
        unsigned int offset = 0;
        for (int i = 0; i < loader->header.entryCount; i++) {
            Entry* entry = (Entry*)((char*)loader->entries + offset);
            if (!loader->reader->vtable->read(loader->reader, entry->data, 0xc34)) return ok;
            entry->stateB = 2;
            offset += 0x14f8;
        }
        loader->reader->vtable->seek(loader->reader, position);
    } else if (loader->header.kind == 2) {
        unsigned int offset = 0;
        for (int i = 0; i < loader->header.entryCount; i++) {
            Entry* entry = (Entry*)((char*)loader->entries + offset);
            entry->stateA = 2;
            offset += 0x14f8;
        }
    }
    unsigned int value;
    loader->reader->vtable->read(loader->reader, &value, 4);
    loader->length = value >> 14;
    loader->mask = value & 0x3fff;
    loader->zero80 = 0;
    loader->reader->vtable->read(loader->reader, loader->work[0], loader->length + 4);
    loader->loaded = 1;
    return 1;
}
