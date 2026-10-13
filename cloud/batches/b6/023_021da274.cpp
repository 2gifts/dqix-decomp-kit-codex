#include <globaldefs.h>
#include "GameState/GameState.h"

struct Obj0204b010 { unsigned char storage[0x20]; };
struct Obj0204b8d0 : Obj0204b010 {};
struct Struct0200fb08;
struct PanelMarker { short x; short y; unsigned char padding4[10]; unsigned char kind; };
struct PanelMap {
    PanelMarker* marker;
    unsigned char padding4[0x1a];
    unsigned char mask;
    unsigned char padding1f;
    unsigned char variant;
};
struct PanelState {
    unsigned char padding0[0xc0];
    PanelMap* map;
    unsigned char paddingc4[0x94];
    Obj0204b8d0 heading;
    unsigned char padding178[0x20];
    Obj0204b8d0 mapLayer;
    Obj0204b8d0 selection;
    unsigned char padding1d8[0xa80];
    signed char page;
    unsigned char paddingc59[0x13c];
    unsigned char option;
    unsigned char paddingd96[6];
    unsigned int flags;
    unsigned char paddingda0[3];
    unsigned char selected;
    unsigned char columns[2];
    unsigned char gridA[6];
    unsigned char gridB[1];
};
struct MapChoice { unsigned char variants[2]; unsigned char mask; };
extern const MapChoice data_ov023_021fd530[] __attribute__((aligned(4)));
extern const unsigned char data_ov023_021fd532[];
void ClearBuffer0204b010(Obj0204b010*, void*);
int NormalizeField5_0200fb08(Struct0200fb08*);
void DispatchEntry0204b8d0(Obj0204b8d0*, unsigned int, int, int, short, short, short, short, unsigned short);
extern "C" void func_0204b04c(void*, int);
extern "C" void func_0204b9b8(void*, short, short, short, short, int);
extern "C" void func_ov003_0215ec68(int, short*, short*);
static inline unsigned char& ReadPanelIndex(unsigned char* values, int index) { return values[index]; }

// USA: func_ov023_021da274
extern "C" ARM void func_ov023_021da274(PanelState* state) {
    short headerY;
    if (state->flags & 0x1000) {
        ClearBuffer0204b010(&state->heading, 0);
        if (state->page >= 1 && state->page <= 8) {
            headerY = (state->page - 1) * 2 + 1;
            short x;
            short destinationY = headerY + 5;
            x = 15;
            short width = 10;
            if (NormalizeField5_0200fb08((Struct0200fb08*)GameState::GetInstance()) == 4) {
                x = 14;
                width = 13;
            }
            DispatchEntry0204b8d0(&state->heading, 0, 0, headerY, x, destinationY, width, 2, 0xffff);
        }
        func_0204b04c(&state->heading, 0);
    }
    if (!(state->flags & 0x2000)) return;
    ClearBuffer0204b010(&state->selection, 0);
    if (state->page == 1) {
        unsigned int entry = 0;
        short x;
        short y;
        if (state->selected == 0) {
            x = 7;
            y = 5;
        } else if (state->selected == 1) {
            entry = 1;
            x = 7;
            y = 11;
        }
        DispatchEntry0204b8d0(&state->selection, entry, 0, 0, x, y, 18, 6, 0xffff);
    } else if (state->page == 2) {
        unsigned int entry = ReadPanelIndex(state->columns, state->selected);
        DispatchEntry0204b8d0(&state->selection, entry, 0, 0, entry * 5 + 4, 5, 4, 9, 0xffff);
    } else if (state->page == 5) {
        DispatchEntry0204b8d0(&state->selection, 0, 0, 0, 4, 5, 24, 9, 0xffff);
        int selected = ReadPanelIndex(state->gridA, state->selected);
        int column = selected % 5;
        int row = selected / 5;
        func_0204b9b8(&state->selection, column * 5 + 4, (row + 1) * 5, 4, 4, 2);
    } else if (state->page == 3) {
        DispatchEntry0204b8d0(&state->selection, 0, 0, 0, 4, 5, 24, 9, 0xffff);
        int selected = ReadPanelIndex(state->gridB, state->selected);
        int column = selected % 5;
        int row = selected / 5;
        func_0204b9b8(&state->selection, column * 5 + 4, (row + 1) * 5, 4, 4, 2);
    } else if (state->page == 8) {
        int i = 0;
        if (state->flags & 0x400000) {
            int height = 10;
            DispatchEntry0204b8d0(&state->selection, 3, 0, 0, 0, 14, 32, height, 0xffff);
        } else {
            int mask = state->map->mask;
            int variant = state->map->variant;
            for (; ; i++) {
                if (!data_ov023_021fd532[i * 3]) break;
                if (mask & data_ov023_021fd532[i * 3]) {
                    DispatchEntry0204b8d0(&state->mapLayer, data_ov023_021fd530[i].variants[variant], 0, 0, 0, 0, 32, 24, 0xffff);
                    break;
                }
            }
            func_0204b9b8(&state->mapLayer, 2, 7, 28, 12, 1);
            PanelMarker* marker = state->map->marker;
            if (marker) {
                short height;
                short width;
                short x = marker->x;
                short y = marker->y;
                func_ov003_0215ec68(marker->kind, &width, &height);
                func_0204b9b8(&state->mapLayer, x >> 3, y >> 3, width >> 3, height >> 3, 0);
            }
        }
        func_0204b04c(&state->mapLayer, 0);
    } else if (state->page == 9) {
        unsigned int entry = 0;
        if (state->option == 0) entry = 1;
        DispatchEntry0204b8d0(&state->selection, entry, 0, 0, 0, 14, 32, 10, 0xffff);
        DispatchEntry0204b8d0(&state->selection, 2, 0, 0, 24, 9, 8, 7, 0xffff);
        func_0204b9b8(&state->mapLayer, 2, 7, 28, 12, 1);
        func_0204b04c(&state->mapLayer, 0);
    }
    func_0204b04c(&state->selection, 0);
}
