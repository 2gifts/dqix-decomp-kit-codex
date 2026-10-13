#include <globaldefs.h>

struct MenuEntry02165f2c {
    short index;
    unsigned short state : 2;
    unsigned short special : 1;
    unsigned short reserved : 13;
    char name[4];
    struct MenuEntry02165f2c* next;
};

struct StateSeq3 { unsigned char v[3]; };
struct StateSeq4 { unsigned char v[4]; };
struct StateSeqTable { char field_0x0[6]; struct StateSeq3 seq; };

struct MenuLists02165f2c {
    struct MenuEntry02165f2c* regular;
    struct MenuEntry02165f2c* special;
    struct MenuEntry02165f2c* head;
    char field_0xc[0x48 - 0xc];
    short regularCount;
    short specialCount;
    short count;
    char field_0x4e[2];
    unsigned char field_0x50 : 6;
    unsigned char useSpecial : 1;
    unsigned char linkByState : 1;
};

extern struct MenuLists02165f2c* data_ov004_0217101c;
extern struct StateSeqTable data_ov004_0216ff98;
extern struct StateSeq4 data_ov004_0216ffa1;

// USA: func_ov004_02165f2c
extern "C" ARM void func_ov004_02165f2c(void) {
    struct MenuLists02165f2c* lists = data_ov004_0217101c;
    struct MenuEntry02165f2c* e;
    struct MenuEntry02165f2c* base;
    struct MenuEntry02165f2c* prev;
    short n;
    unsigned char* p;
    short i;
    int t;
    if (lists->linkByState == 1) {
        base = lists->regular;
        lists->head = base;
        lists->count = lists->regularCount;
        n = lists->regularCount;
        struct StateSeq3 a = data_ov004_0216ff98.seq;
        prev = 0;
        struct StateSeq4 b = data_ov004_0216ffa1;
        p = a.v;
        if (lists->useSpecial == 1) {
            base = lists->special;
            lists->head = base;
            p = b.v;
            lists->count = lists->specialCount;
            n = lists->specialCount;
        }
        for (; (t = *p) != 0xff; p++) {
            e = base;
            for (i = 0; i < n; e++, i++) {
                if (e->next != 0) continue;
                if (e->special && t == 1) {
                    if (e->state == 0 || e->state == t) {
                        if (prev) prev->next = e; else lists->head = e;
                        prev = e;
                    }
                } else if (e->state == t) {
                    if (prev) prev->next = e; else lists->head = e;
                    prev = e;
                }
            }
        }
    } else if (lists->linkByState == 0) {
        e = lists->regular;
        lists->head = e;
        lists->count = lists->regularCount;
        short total = lists->regularCount;
        if (lists->useSpecial == 1) {
            e = lists->special;
            lists->head = e;
            lists->count = lists->specialCount;
            total = lists->specialCount;
        }
        for (short k = 0; k < total - 1; k++) {
            struct MenuEntry02165f2c* nx = e + 1;
            e->next = nx;
            e = nx;
        }
    }
}
