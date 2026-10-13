#include <globaldefs.h>

struct Event0202cb50 {
    unsigned short field0;
    unsigned short result;
    unsigned short type;
};

struct State0202cb50 {
    char reserved0[0x10];
    int state;
    int field14;
    void (*callback)(Event0202cb50*);
    char reserved1c[0x18];
    int mode;
};

extern State0202cb50 data_021015a0;
extern int data_021023c0;
extern int data_020ef7a8;
extern int data_02102be0;
void SetField0x48UnlessState9Or10(int);
int InvokeWithFullMaskType2(int, int);
extern "C" int func_020d5d34(void*, int, int, int, int);

// USA: func_0202cb50
extern "C" ARM void func_0202cb50(Event0202cb50* event)
{
    int result = event->result;
    if (result) {
        SetField0x48UnlessState9Or10(result);
        data_021015a0.state = 9;
    } else {
        switch (event->type) {
        case 10:
            if (data_021015a0.mode == 2) {
                if (data_021015a0.state == 4) {
                    data_021015a0.state = 6;
                    int ok = InvokeWithFullMaskType2((int)&data_021023c0, 13);
                    if (!ok) ok = 1;
                    else {
                        SetField0x48UnlessState9Or10(ok);
                        ok = 0;
                    }
                    if (!ok) data_021015a0.state = 9;
                    break;
                }
                if (data_021015a0.state != 6) goto fallback;
                break;
            } else if (data_021015a0.mode == 4) {
                int status = func_020d5d34(&data_02102be0, 13, 15, 0x74, data_020ef7a8);
                if (status) {
                    SetField0x48UnlessState9Or10(status);
                    data_021015a0.state = 9;
                } else {
                    data_021015a0.state = 5;
                    if (data_021015a0.callback) data_021015a0.callback(event);
                }
                break;
            }
        fallback:
            data_021015a0.state = 4;
            if (data_021015a0.callback) data_021015a0.callback(event);
            break;
        case 11:
        case 12:
        case 13:
            break;
        }
    }
}
