#include <globaldefs.h>
#include "Resource/Script.h"
struct Record12 {
    unsigned short field_0x0a : 11;
    unsigned short field_0x0b : 5;
    unsigned short field_0x2;
    unsigned int field_0x4a : 5;
    unsigned int field_0x4b : 7;
    unsigned int field_0x4c : 8;
    unsigned int field_0x4d : 5;
    unsigned int field_0x4e : 3;
    unsigned int field_0x4f : 4;
    unsigned short field_0x8;
    unsigned short field_0xa;
};
struct RecordList0209a55c;
void AppendRecord12Capped(RecordList0209a55c*, Record12*);
extern RecordList0209a55c* data_02109ba4;
// USA: func_0209a218
extern "C" ARM int func_0209a218(Script::Parameter* args) {
    Record12 record;
    record.field_0x0a = args[0].ToInt();
    record.field_0x4a = args[1].ToInt();
    record.field_0x4b = args[2].ToInt();
    record.field_0x2 = args[3].ToInt();
    record.field_0x0b = args[4].ToInt();
    record.field_0x4c = args[5].ToInt();
    record.field_0x8 = args[6].ToInt();
    record.field_0xa = args[7].ToInt();
    record.field_0x4d = args[8].ToInt();
    record.field_0x4f = 0;
    record.field_0x4e = 0;
    if (record.field_0x4c != 0) AppendRecord12Capped(data_02109ba4, &record);
    return 1;
}
