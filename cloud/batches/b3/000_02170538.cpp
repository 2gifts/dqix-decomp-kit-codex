#include <globaldefs.h>

struct Struct0203c1c8 { int x, y, mode, count, enabled; short field14; };
struct Obj0203c108;
struct CommandMenu {
    unsigned char padding0[0x10];
    signed char commands[8];
    signed char selected;
    unsigned char padding19[3];
    signed char mode;
    signed char field1d;
    unsigned char padding1e[6];
    unsigned char flags;
    unsigned char padding25;
    unsigned short field26;
    unsigned int selectionFlags;
    short field2c;
    signed char field2e;
    unsigned char padding2f[13];
    unsigned int cachedFlags;
    unsigned char padding40[4];
    int x;
    int y;
    int entry;
    Struct0203c1c8 description;
    Struct0203c1c8 label;
    unsigned char padding80[7];
    unsigned char active;
    unsigned char padding88[0x3bb];
    signed char field443;
};
struct MenuStyles {
    unsigned char padding0[0x80];
    int special;
    int field84;
    int normal;
};
extern MenuStyles data_ov000_021832f4;
extern "C" int func_ov000_0217f5dc(CommandMenu*);
extern "C" int func_ov000_02170c7c(CommandMenu*);
extern "C" int func_ov000_02171a74(CommandMenu*, char*);
void DispatchEntriesOnce021704d8(Struct0203c1c8*, int*, int*, int*);
int HasAnyFlags_021719f8_021719f8(int*);
void SwapGlobalEntry0203c108(Obj0203c108*, char*);
void ReleaseGlobalEntry0203c198(char*);

// USA: func_ov000_02170538
extern "C" ARM void func_ov000_02170538(CommandMenu* menu, int x, int y, int style) {
    if (menu->entry < 0 || !func_ov000_0217f5dc(menu)) return;
    int mode = func_ov000_02170c7c(menu);
    if (menu->flags & 4) {
        int positionsX[2];
        positionsX[0] = menu->x + 0x25;
        positionsX[1] = menu->x + 0x26;
        int positionsY[2];
        positionsY[0] = menu->y + 0xb;
        positionsY[1] = menu->y + 0xb;
        int modes[2];
        modes[1] = data_ov000_021832f4.special;
        modes[0] = mode;
        DispatchEntriesOnce021704d8(&menu->label, positionsX, positionsY, modes);
    } else {
        int positionsX[2];
        positionsX[0] = menu->x + 0x28 + x;
        positionsX[1] = menu->x + 0x30 + x;
        int positionsY[2];
        positionsY[0] = menu->y + 8 + y;
        positionsY[1] = menu->y + 8 + y;
        int modes[2];
        modes[1] = data_ov000_021832f4.normal;
        modes[0] = mode;
        DispatchEntriesOnce021704d8(&menu->label, positionsX, positionsY, modes);
        if (menu->commands[menu->selected] == 0x67) return;
        int descriptionX[2] = {menu->x + 0xb3 + x, menu->x + 0xb4 + x};
        int descriptionY[2][2] = {{menu->y + 0xf + y, menu->y + 0xf + y},
                                  {menu->y + 8 + y, menu->y + 8 + y}};
        int previous = menu->cachedFlags & 0x80019;
        int current = menu->selectionFlags & 0x80019;
        int alternate = 1;
        if (HasAnyFlags_021719f8_021719f8((int*)menu)) {
            if (previous != current) {
                char text[0x80] = {};
                if (func_ov000_02171a74(menu, text) >= 0)
                    SwapGlobalEntry0203c108((Obj0203c108*)&menu->description, text);
                menu->cachedFlags = menu->selectionFlags;
            }
            DispatchEntriesOnce021704d8(&menu->description, descriptionX, descriptionY[0], modes);
        } else {
            if (previous != current && (menu->cachedFlags & 0x80019)) {
                ReleaseGlobalEntry0203c198((char*)&menu->description);
                menu->cachedFlags = menu->selectionFlags;
                menu->field1d = -1;
                menu->field2c = -1;
                menu->field2e = -1;
                menu->field26 = 0;
            }
            int command = menu->commands[menu->selected];
            int displayMode = 3;
            if (command != 0x66) displayMode = 0;
            if (menu->field26 == 3) alternate = 0;
            if (menu->mode == 6) alternate = 0;
            if (command < 0x64) {
                if (menu->active) alternate = 0;
                else return;
            }
            modes[0] = displayMode;
            if (style != 2) {
                if (menu->field443 != 5) alternate = 0;
                DispatchEntriesOnce021704d8(&menu->description, descriptionX, descriptionY[alternate], modes);
            }
        }
    }
}
