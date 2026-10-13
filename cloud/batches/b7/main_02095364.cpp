#include <globaldefs.h>
#include "Resource/Script.h"

struct State02095364 {
    unsigned char field0;
    unsigned char countingOnly;
    unsigned char selectedFlag;
    unsigned char field3;
    unsigned short field4;
    unsigned short count;
    unsigned int field8;
    unsigned int fieldc;
    unsigned int field10;
};

struct Entry02095364 {
    unsigned short id : 9;
    unsigned short flag9 : 1;
    unsigned short flag10 : 1;
    unsigned short flag11 : 1;
    unsigned short flag12 : 1;
    unsigned short reserved : 3;
};

struct Table02095364 {
    char reserved[0xa8];
    Entry02095364* entries;
};

extern State02095364 data_02109404 __attribute__((aligned(4)));
extern Table02095364 data_02109418;
extern short data_020f13d8;

static inline short GetSelectedId02095364()
{
    return data_020f13d8;
}

// USA: func_02095364
extern "C" ARM int func_02095364(Script::Parameter* params)
{
    if (data_02109404.countingOnly) {
        ++data_02109404.count;
        return 1;
    }
    unsigned short id = params[0].ToInt();
    unsigned short flag9 = params[5].ToInt();
    unsigned short flag11 = params[7].ToInt();
    unsigned short flag10 = params[8].ToInt();
    unsigned short flag12 = params[9].ToInt();
    Entry02095364* entries = data_02109418.entries;
    if (entries) {
        Entry02095364* entry = &entries[data_02109404.count];
        entry->id = id;
        entry->flag9 = flag9;
        entry->flag11 = flag11;
        entry->flag10 = flag10;
        entry->flag12 = flag12;
    }
    unsigned int count = data_02109404.count;
    int selectedId = data_020f13d8;
    ++count;
    data_02109404.count = count;
    if (selectedId >= 0 && id == selectedId) data_02109404.selectedFlag = flag10 != 0;
    return 1;
}
