#include <globaldefs.h>
struct Resources020ae53c { unsigned char pad0[0x4458]; int mode; };
struct Flags020ae53c { unsigned char pad0[0x8c]; unsigned char flags[1]; };
struct Map020ae53c { unsigned short id; };
extern "C" Resources020ae53c* func_ov017_0218b5b0();
extern "C" Flags020ae53c* func_0205ec34();
extern "C" Map020ae53c* func_02012fe4();
int TestBitInByteArray(int, unsigned char*, int);
void ToggleElemKey2FlagInMap2010(int);
void SetElemFlag8IfOverlayActive020ae730(int);
void SetElemFlag0x4PairAndNotify17(int);
void SetElemFlag0x4QuadAndNotify17(int);
void NotifyElemAndOverlay17OnFlag020ae990(int);
void SyncEightElementFlags();
void SetOrClearElemFlag0x4ByKeys(int, int, short);
void SetFlagBitAndNotifyOverlay17(int, int);
extern "C" void func_020aee04(int, int);
// USA: func_020ae53c
extern "C" ARM void func_020ae53c(int mode) {
    Flags020ae53c* flags;
    Resources020ae53c* resources = func_ov017_0218b5b0();
    flags = func_0205ec34();
    func_02012fe4();
    resources->mode = mode;
    ToggleElemKey2FlagInMap2010(!TestBitInByteArray((int)flags, flags->flags, 0x33e));
    SetElemFlag8IfOverlayActive020ae730((unsigned char)TestBitInByteArray((int)flags, flags->flags, 0x344));
    SetElemFlag0x4PairAndNotify17((unsigned char)TestBitInByteArray((int)flags, flags->flags, 0x347));
    SetElemFlag0x4QuadAndNotify17((unsigned char)TestBitInByteArray((int)flags, flags->flags, 0x34a));
    NotifyElemAndOverlay17OnFlag020ae990((unsigned char)TestBitInByteArray((int)flags, flags->flags, 0x36c));
    SyncEightElementFlags();
    mode = TestBitInByteArray((int)flags, flags->flags, 0x385);
    if (func_02012fe4()->id == 0x1cea) {
        for (int key = 0x4e; key <= 0x58; key++) SetOrClearElemFlag0x4ByKeys((unsigned char)mode, 0, key & 0xffff);
        SetFlagBitAndNotifyOverlay17(0x47, mode != 0);
    }
    func_020aee04(0, TestBitInByteArray((int)flags, flags->flags, 0x386));
    resources->mode = 1;
}
