#include <globaldefs.h>
#include "std_library_functions.h"
#include "Memory/SafeAllocator.h"

struct StringRecord02097054 {
    int field_0x0;
    short index;
    char field_0x6[6];
    unsigned char stringCount;
    char field_0xd[0x1f];
    char** strings;
};
struct Source02097054 {
    unsigned short count;
    unsigned short entryCount;
    unsigned int flags;
    StringRecord02097054 records[1];
};
struct Container02097054 {
    unsigned short count;
    unsigned short entryCount;
    unsigned int flags;
    StringRecord02097054* records;
    char* strings;
};
struct ComputeAlignedBufferSizeHeader { unsigned short count, entryCount; };
struct ForEachNode02097000Node { char field_0x0[0x30]; };
struct ForEachNode02097000Container { unsigned short count; char field_0x2[6]; ForEachNode02097000Node* nodes; };
struct BinarySearch020971a8Node { char field_0x0[0x30]; };
struct BinarySearch020971a8Container { unsigned short count; char field_0x2[6]; BinarySearch020971a8Node* nodes; };
struct Struct02096f80 { char field_0x0[12]; char* strings; };
union ContainerViews02097054 {
    Container02097054 container;
    ComputeAlignedBufferSizeHeader header;
    ForEachNode02097000Container iteration;
    BinarySearch020971a8Container search;
    Struct02096f80 stringTable;
};
struct Output02097054 { unsigned short count; char field_0x2[6]; StringRecord02097054* record; int field_0xc; };
int ComputeAlignedBufferSize(ComputeAlignedBufferSizeHeader*);
int ForEachNode02097000(ForEachNode02097000Container*, void (*)(ForEachNode02097000Container*, ForEachNode02097000Node*));
BinarySearch020971a8Node* BinarySearch020971a8(BinarySearch020971a8Container*, int, int (*)(BinarySearch020971a8Node*));
short GetShortAt0x4(unsigned char*);
char* AllocateStringCopy020da150(SafeAllocator*, const char*);
extern "C" char* func_02096f80(Struct02096f80*, char*, int);

// USA: func_02097054
extern "C" ARM void func_02097054(Output02097054* output, SafeAllocator* allocator, Source02097054* source, int unused, short index) {
    StringRecord02097054* record;
    ContainerViews02097054 views;
    memset(output, 0, 16);
    if (index < 0) return;
    if (source != NULL) {
    memcpy(&views, source, 8);
    views.container.records = source->records;
    views.container.strings = (char*)source + (ComputeAlignedBufferSize(&views.header) + 8);
    if (!(views.container.flags >> 31)) {
        ForEachNode02097000(&views.iteration, NULL);
        views.container.flags = (views.container.flags & 0x7fffffff) | 0x80000000;
        source->flags = (source->flags & 0x7fffffff) | 0x80000000;
    }
    }
    char** strings;
    char** offsets;
    int position;
    short count;
    StringRecord02097054* selected = (StringRecord02097054*)BinarySearch020971a8(&views.search, index, (int (*)(BinarySearch020971a8Node*))GetShortAt0x4);
    record = (StringRecord02097054*)allocator->Allocate(0x30);
    if (selected == NULL || record == NULL) return;
    memcpy(record, selected, 0x30);
    count = record->stringCount;
    strings = (char**)allocator->Allocate(count * 4);
    offsets = (char**)(views.container.records + views.container.count) + (int)selected->strings;
    record->strings = strings;
    for (position = 0; position < count; position++) {
        *strings++ = AllocateStringCopy020da150(allocator, func_02096f80(&views.stringTable, *offsets++, 0));
    }
    output->count = 1;
    output->record = record;
}
