#include <globaldefs.h>
#include "GameState/GameState.h"

extern "C" void *func_02012fe4(void);
extern "C" int _s32_div_f(int a, int b);
int IsInRange0201b588(int zoneId);
int IsInRange0201b5b0(int zoneId);
void SetElementFields0202756c(void *receiver, int x, int y, int elementIndex, unsigned char image, unsigned char palette,
                              unsigned short tile, unsigned char flags, int scaleX, int scaleY);

struct GrottoCell {
    unsigned char unknown0[0x24];
    int x;
    unsigned char unknown28[4];
    int y;
    unsigned char unknown30[4];
};

struct GrottoView {
    unsigned char unknown0[0x1d8];
    unsigned char firstCol;
    unsigned char firstRow;
    unsigned char secondCol;
    unsigned char secondRow;
    GrottoCell cells[2];
};

struct ZoneRecord {
    unsigned short zoneId;
    unsigned char unknown2[0x23ec - 2];
    GrottoView grotto;
};

struct ElementReceiver {
    unsigned char unknown0[0x38];
    fix32_t scale;
    unsigned char unknown3c[8];
    int offsetX;
    int offsetY;
    unsigned char unknown4c[0x9ba - 0x4c];
    unsigned char flag9ba;
};

// USA: func_02024000
extern "C" ARM void TODO_Name_02024000(void *receiver) {
    ElementReceiver *state = static_cast<ElementReceiver *>(receiver);
    GameState *game = GameState::GetInstance();
    ZoneRecord *zone = static_cast<ZoneRecord *>(func_02012fe4());
    unsigned short zoneId = zone->zoneId;
    if (!IsInRange0201b588(zoneId)) {
        return;
    }
    SetElementFields0202756c(receiver, 0xbc000, 0x3000, 26, 0x42, 0, 0xff, 0xff, 0x1000, 0x1000);
    if (!IsInRange0201b5b0(zoneId)) {
        return;
    }
    unsigned char *bits = reinterpret_cast<unsigned char *>(game) + 0x6fcc + (zoneId % 20 - 1) * 32;
    GrottoView *grotto = &zone->grotto;
    GrottoCell *cell = grotto->cells;
    GrottoCell *cell2 = cell + 1;
    unsigned char firstCol = grotto->firstCol;
    unsigned char firstRow = grotto->firstRow;
    unsigned char secondCol = grotto->secondCol;
    unsigned char secondRow = grotto->secondRow;
    bool firstMatch = false;
    bool secondMatch = false;
    for (int row = 0; row < 16; row++) {
        int lo = row * 2;
        for (int col = 0; col < 16; col++) {
            bool set;
            if (col < 8) {
                set = (bits[lo] & (1 << col)) != 0;
            } else {
                set = (bits[lo + 1] & (1 << (col - 8))) != 0;
            }
            if (col == firstCol && row == firstRow && set) {
                firstMatch = true;
            }
            if (col == secondCol && row == secondRow && set) {
                secondMatch = true;
            }
        }
    }
    if (firstMatch) {
        int x = FIX32_MULTIPLY(cell->x, state->scale) - (state->offsetX << 12) + 0x7c000;
        int y = FIX32_MULTIPLY(cell->y, state->scale) - (state->offsetY << 12) + 0x5c000;
        SetElementFields0202756c(receiver, x, y, 24, 0x40, 1, 0xff, 0xff, 0x1000, 0x1000);
    }
    if (!secondMatch && !state->flag9ba) {
        return;
    }
    int x = FIX32_MULTIPLY(cell2->x, state->scale) - (state->offsetX << 12) + 0x7c000;
    int y = FIX32_MULTIPLY(cell2->y, state->scale) - (state->offsetY << 12) + 0x5c000;
    SetElementFields0202756c(receiver, x, y, 25, 0x41, 1, 0xff, 0xff, 0x1000, 0x1000);
}
