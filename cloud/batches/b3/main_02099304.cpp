#include <globaldefs.h>

class Script {
public:
    struct OpcodeLookupEntry;
    void Initialize();
    void SetOpcodeLookup(OpcodeLookupEntry* lookup);
    void Load(const void* data, unsigned int size);
    void Execute();
};

struct ScriptRequest {
    unsigned int lo : 12;
    unsigned int mid : 4;
    unsigned int hi : 5;
    unsigned int rest : 10;
    unsigned int top : 1;
    int offset;
    signed char mode;
};

struct StoreStruct;
struct Obj02046574;

struct ScriptGlobal {
    unsigned char flags;
    void* buffer;
    ScriptRequest* request;
};

extern ScriptGlobal data_021098e8;
extern char data_020f1504[];

extern "C" void* func_0202ae18(void);
extern "C" int func_0202c508(void* gs);
extern "C" void func_02046380(void);
extern "C" void* memset(void* dst, int value, unsigned int length);
void RegisterSlotA020cf0fc(int out);
StoreStruct* GetGlobalField0x1c020421a0(void);
void StoreInArray0x8b0(StoreStruct* store, int index, int value);
int CalculateAge(int a, int b, int c);
void SetIndexedName02046574(Obj02046574* obj, int index, char* name);

// USA: func_02099304
extern "C" ARM int LoadScriptBlock02099304(ScriptRequest* request, char* name, const void* data, unsigned int size, void* buffer, int bufferSize, unsigned char flags, char* nameA, char* nameB) {
    void* gs = func_0202ae18();
    int slot[4];
    unsigned char scriptBuf[0x430];

    if (request == 0 || name == 0) {
        return 0;
    }
    if (bufferSize < 0x190) {
        return 0;
    }
    memset(buffer, 0, bufferSize);
    data_021098e8.request = request;
    data_021098e8.buffer = buffer;
    data_021098e8.flags = 0;
    if (flags == 0) {
        data_021098e8.flags |= 0x10;
    } else {
        if (flags & 2) {
            if (func_0202c508(gs)) {
                RegisterSlotA020cf0fc((int)slot);
                if (slot[1] == request->mid && slot[2] == request->hi) {
                    data_021098e8.flags |= 1;
                }
                if (slot[1] == 1 && slot[2] == 1) {
                    data_021098e8.flags |= 2;
                } else if (slot[1] == 0xc && slot[2] == 0x19) {
                    data_021098e8.flags |= 8;
                } else if (slot[1] == 2 && slot[2] == 0xe) {
                    data_021098e8.flags |= 4;
                }
            }
        }
        if ((flags & 1) && request->mode != 0) {
            data_021098e8.flags |= 0x20;
        }
    }

    StoreStruct* store = GetGlobalField0x1c020421a0();
    func_02046380();
    unsigned int mid = request->mid;
    unsigned int hi = request->hi;
    StoreInArray0x8b0(store, 1, mid);
    StoreInArray0x8b0(store, 2, hi);
    if (request->top) {
        int age;
        if (((request->offset << 23) >> 23) < 0) {
            age = 0;
        } else {
            age = CalculateAge(request->lo, request->mid, request->hi);
            if (age < 0) {
                age = 0;
            }
        }
        StoreInArray0x8b0(store, 0, age);
    }
    SetIndexedName02046574((Obj02046574*)store, 0, name);
    SetIndexedName02046574((Obj02046574*)store, 1, nameA);
    SetIndexedName02046574((Obj02046574*)store, 2, nameB);

    ((Script*)scriptBuf)->Initialize();
    ((Script*)scriptBuf)->SetOpcodeLookup((Script::OpcodeLookupEntry*)data_020f1504);
    ((Script*)scriptBuf)->Load(data, size);
    ((Script*)scriptBuf)->Execute();
    return 1;
}
