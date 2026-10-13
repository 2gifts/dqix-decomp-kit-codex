#include <globaldefs.h>
struct Item02047554 { int field_0x0, field_0x4; };
struct Entry02047554 {
    unsigned char field_0x0, row, count, field_0x3;
    Item02047554* items;
};
struct Object02047554 {
    char padding_0x0[8];
    Entry02047554* entries;
    char padding_0xc[0x10];
    int x, y, z;
    char padding_0x28[0xc];
    int field_0x34, field_0x38, field_0x3c;
    char padding_0x40[0x42];
    short count;
    unsigned char enabled : 1;
    unsigned char ready : 1;
    unsigned char otherFlags : 6;
};
extern "C" void func_02047718(Object02047554*, Item02047554*, int);
#define REG_GX_IDENTITY (*(volatile int*)0x04000444)
#define REG_GX_MODE (*(volatile int*)0x04000448)
#define REG_GX_TRANSLATE (*(volatile int*)0x04000470)
#define REG_GX_SCALE (*(volatile int*)0x0400046c)
#define REG_GX_POLYGON (*(volatile int*)0x040004c0)
#define REG_GX_COLOR (*(volatile int*)0x040004c4)
// USA: func_02047554
extern "C" ARM void func_02047554(Object02047554* object, int index, int argument) {
    if (!object->enabled || !object->ready || object->count <= 0) return;
    Entry02047554* entry = &object->entries[index];
    if (!entry->items || !entry->count) return;
    REG_GX_IDENTITY = 0;
    REG_GX_POLYGON = 0x7fffffff;
    REG_GX_COLOR = 0x4210;
    int row = entry->row;
    int y = object->y;
    int rowHeight = object->field_0x38;
    int z = object->z;
    int x = object->x;
    rowHeight = y + row * rowHeight;
    REG_GX_TRANSLATE = x;
    REG_GX_TRANSLATE = rowHeight;
    REG_GX_TRANSLATE = z;
    int scaleY = object->field_0x38;
    int scaleZ = object->field_0x3c;
    int scaleX = object->field_0x34;
    scaleY = -scaleY;
    REG_GX_SCALE = scaleX;
    REG_GX_SCALE = scaleY;
    REG_GX_SCALE = scaleZ;
    Item02047554* item = entry->items;
    for (int i = 0; i < entry->count; i++, item++) func_02047718(object, item, argument);
    REG_GX_MODE = 1;
}
