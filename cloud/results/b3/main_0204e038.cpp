#include <globaldefs.h>

struct Obj0204e038 {
    unsigned char pad0[8];
    unsigned char* data;
    unsigned char pad1[0xa8 - 0xc];
    short f0xa8;
    short f0xaa;
};

// USA: func_0204e038
ARM void func_0204e038(struct Obj0204e038* obj, unsigned char* mask, short startY, short startX, int boxH, int boxW, int colorHi, int colorLo) {
    if (mask == 0) {
        return;
    }

    unsigned char bitIndex = 0;
    short cachedW = obj->f0xa8;
    short cachedH = obj->f0xaa;
    short j;
    for (j = 0; j < boxW; j++) {
        short cx = startX + j;
        short tileCol = cx >> 3;
        int shiftX = (short)((cx & 7) << 2);
        int inRangeX = tileCol >= 0 && tileCol < cachedH;
        int rowBase = tileCol * obj->f0xa8;

        short k;
        for (k = 0; k < boxH; k++) {
            if (inRangeX != 0) {
                if (*mask & (1 << bitIndex)) {
                    short cy = startY + k;
                    int pixOffY = cy & 7;
                    short tileRow = cy >> 3;
                    int inRangeY = tileRow >= 0 && tileRow < cachedW;
                    int combined = inRangeY | inRangeX;
                    if (combined != 0) {
                        int off = shiftX + ((tileRow + rowBase) << 5) + (pixOffY >> 1);
                        volatile unsigned char* base = obj->data;
                        int v = base[(short)off];
                        if ((cy & 1) != 0) {
                            v = v & 0xf;
                            base[off] = v;
                            v = v | colorHi;
                            base[off] = v;
                        } else {
                            v = v & 0xf0;
                            base[off] = v;
                            v = (v & 0xff) | colorLo;
                            base[off] = v;
                        }
                    }
                }
            }
            bitIndex++;
            if (bitIndex >= 8) {
                bitIndex = 0;
                mask++;
            }
        }
    }
}
