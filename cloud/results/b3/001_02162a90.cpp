#include <globaldefs.h>

struct BackgroundControl {
    unsigned short priority : 2;
    unsigned short charBase : 4;
    unsigned short mosaic : 1;
    unsigned short colorMode : 1;
    unsigned short screenBase : 5;
    unsigned short overflow : 1;
    unsigned short screenSize : 2;
};
struct BackgroundState { char pad0[0x98]; BackgroundControl main[4]; char pad1[8]; BackgroundControl sub[4]; };
struct BackgroundGlobals { char pad0[0x494]; int mainBanks; char pad1[8]; int mainLayers; int subBanks; char pad2[8]; int subLayers; };
extern BackgroundState data_ov001_02165cb8;
extern BackgroundGlobals data_ov001_021658b8;
extern "C" int func_ov017_021d60f4();
extern "C" void ReleaseMainBGVRAMBanks();
extern "C" void MapVRAMBanksToMainBG(int);
extern "C" void ReleaseSubBGVRAMBanks();
extern "C" void MapVRAMBanksToSubBG(int);
void SetBg3ControlFields_02162678(int, int, int, int);
void SetSubBg0Control_0215e758(int, int, int, int, int);
void SetSubBg1Control_0215e78c(int, int, int, int, int);
void SetSubBg3ControlFields_021628d4(int, int, int, int);
#define MAIN_BG_CONTROL ((unsigned short*)0x04000008)
#define SUB_BG2_CONTROL (*(unsigned short*)0x0400100c)
#define MAIN_DISPLAY_CONTROL (*(unsigned int*)0x04000000)
#define SUB_DISPLAY_CONTROL (*(unsigned int*)0x04001000)

// USA: func_ov001_02162a90
extern "C" ARM int func_ov001_02162a90() {
    int display = func_ov017_021d60f4();
    if (display == 0) {
        ReleaseMainBGVRAMBanks();
        MapVRAMBanksToMainBG(data_ov001_021658b8.mainBanks);
        BackgroundControl bg0 = data_ov001_02165cb8.main[0];
        BackgroundControl bg1 = data_ov001_02165cb8.main[1];
        BackgroundControl bg2 = data_ov001_02165cb8.main[2];
        BackgroundControl bg3 = data_ov001_02165cb8.main[3];
        MAIN_BG_CONTROL[0] = (MAIN_BG_CONTROL[0] & 0x43) | (bg0.screenSize << 14) | (bg0.colorMode << 7) | (bg0.screenBase << 8) | (bg0.charBase << 2) | (bg0.overflow << 13);
        MAIN_BG_CONTROL[1] = (MAIN_BG_CONTROL[1] & 0x43) | (bg1.screenSize << 14) | (bg1.colorMode << 7) | (bg1.screenBase << 8) | (bg1.charBase << 2) | (bg1.overflow << 13);
        MAIN_BG_CONTROL[2] = (MAIN_BG_CONTROL[2] & 0x43) | (bg2.screenSize << 14) | (bg2.colorMode << 7) | (bg2.screenBase << 8) | (bg2.charBase << 2);
        SetBg3ControlFields_02162678(bg3.screenSize, bg3.colorMode, bg3.screenBase, bg3.charBase);
        MAIN_DISPLAY_CONTROL = (MAIN_DISPLAY_CONTROL & ~0x1f00) | (data_ov001_021658b8.mainLayers << 8);
    } else if (display == 1) {
        ReleaseSubBGVRAMBanks();
        MapVRAMBanksToSubBG(data_ov001_021658b8.subBanks);
        BackgroundControl bg0 = data_ov001_02165cb8.sub[0];
        BackgroundControl bg1 = data_ov001_02165cb8.sub[1];
        BackgroundControl bg2 = data_ov001_02165cb8.sub[2];
        BackgroundControl bg3 = data_ov001_02165cb8.sub[3];
        SetSubBg0Control_0215e758(bg0.screenSize, bg0.colorMode, bg0.screenBase, bg0.charBase, bg0.overflow);
        SetSubBg1Control_0215e78c(bg1.screenSize, bg1.colorMode, bg1.screenBase, bg1.charBase, bg1.overflow);
        SUB_BG2_CONTROL = (SUB_BG2_CONTROL & 0x43) | (bg2.screenSize << 14) | (bg2.colorMode << 7) | (bg2.screenBase << 8) | (bg2.charBase << 2);
        SetSubBg3ControlFields_021628d4(bg3.screenSize, bg3.colorMode, bg3.screenBase, bg3.charBase);
        SUB_DISPLAY_CONTROL = (SUB_DISPLAY_CONTROL & ~0x1f00) | (data_ov001_021658b8.subLayers << 8);
    }
    return 1;
}
