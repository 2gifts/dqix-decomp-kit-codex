#include <globaldefs.h>

struct VramPool020bb588 {
    unsigned int freeStart_;
    unsigned int freeEnd_;
    unsigned int usable_;
    char pad_c[4];
    unsigned short pairingKind_;
    char pad_12[2];
    unsigned int poolBase_;
};

extern struct VramPool020bb588 data_020f1f2c;
extern struct VramPool020bb588 data_020f1f44;
extern struct VramPool020bb588* data_020f1ef8[2];
extern struct VramPool020bb588* data_020f1f00[5];

// USA: func_020bb588
extern "C" ARM unsigned int func_020bb588(unsigned int amount, bool paired, unsigned int defaultOffset) {
    unsigned int alignedSize;
    unsigned int chosenOffset = defaultOffset;
    if (amount == 0)
        alignedSize = 0x10;
    else
        alignedSize = (amount + 0xf) & ~0xf;

    if (alignedSize >= 0x7fff0)
        return 0;

    bool success;
    if (paired) {
        struct VramPool020bb588** table = data_020f1ef8;
        struct VramPool020bb588* poolThree = &data_020f1f44;
        struct VramPool020bb588* poolOne = &data_020f1f2c;
        int pass = 0;
        do {
            struct VramPool020bb588* secondaryPool;
            struct VramPool020bb588* primaryPool;
            unsigned int start;
            primaryPool = table[pass];
            if (primaryPool->usable_ == 0)
                continue;
            if (primaryPool->freeEnd_ - primaryPool->freeStart_ < alignedSize)
                continue;
            switch (primaryPool->pairingKind_) {
            case 0:
                secondaryPool = poolOne;
                break;
            case 3:
                secondaryPool = poolThree;
                break;
            default:
                secondaryPool = 0;
                break;
            }
            if (secondaryPool->usable_ == 0)
                continue;
            if (secondaryPool->freeEnd_ - secondaryPool->freeStart_ >= (alignedSize >> 1)) {
                start = primaryPool->freeStart_;
                success = true;
                primaryPool->freeStart_ += alignedSize;
                secondaryPool->freeStart_ += (alignedSize >> 1);
                chosenOffset = start + primaryPool->poolBase_;
                goto end;
            }
        } while (pass++, pass < 2);
        success = false;
    } else {
        int pass = 0;
        do {
            struct VramPool020bb588* pool;
            pool = data_020f1f00[pass];
            if (pool->usable_ == 0)
                continue;
            if (pool->freeEnd_ - pool->freeStart_ >= alignedSize) {
                success = true;
                unsigned int newEnd = pool->freeEnd_ - alignedSize;
                pool->freeEnd_ = newEnd;
                chosenOffset = newEnd + pool->poolBase_;
                goto end;
            }
        } while (pass++, pass < 5);
        success = false;
    }

end:
    if (!success)
        return 0;

    return ((alignedSize >> 4) << 16) | ((chosenOffset << 13) >> 16) | (paired << 31);
}
