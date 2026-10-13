#include <globaldefs.h>
#include <System/Cache.h>
// USA: func_0203b97c
extern "C" ARM void func_0203b97c(void*, unsigned short* destination, int width, int height, int x, int y, const unsigned short* source, unsigned int sourceWidth, unsigned int sourceHeight, unsigned short offset) {
    if (width <= x || height <= y) return;
    {
        int tileRow;
        unsigned int sourceRow;
        int targetY;
        unsigned int tilesPerRow = (unsigned int)width >> 5;
        for (unsigned int row = 0; row < sourceHeight; row++) {
            sourceRow = row * sourceWidth;
            targetY = y + row;
            for (unsigned int column = 0; column < sourceWidth; column++) {
                tileRow = (targetY / 32) * tilesPerRow;
                int targetX = x + column;
                if (targetX >= 0 && targetY >= 0 && width > targetX && height > targetY) {
                    int pixelIndex = targetX + (targetY << 5);
                    int targetIndex = (tileRow + targetX / 32) * 0x3e0 + pixelIndex;
                    destination[targetIndex] = source[column + sourceRow];
                    destination[targetIndex] += offset;
                }
            }
        }
    }
    CleanInvalidateCacheRange(destination, height * (width << 1));
}
