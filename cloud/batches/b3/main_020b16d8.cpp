#include <globaldefs.h>

struct TileCtx020b16d8 {
    void* dest;             // 0x0
    unsigned int width;     // 0x4
    unsigned int height;    // 0x8
    unsigned char depth;    // 0xc
    unsigned char padD[3];  // 0xd..0xf
    int field10;            // 0x10
    int field14;            // 0x14
};

struct TileBlob020b16d8 {
    unsigned char b0;       // 0x0
    unsigned char b1;       // 0x1
    unsigned char pad2[4];  // 0x2..0x5
    unsigned char b6;       // 0x6
};

struct TileSet020b16d8 {
    int pad0;                       // 0x0
    int pad4;                       // 0x4
    struct TileBlob020b16d8* blob;  // 0x8
};

struct TileSrc020b16d8 {
    struct TileSet020b16d8* set;    // 0x0
};

struct TileRef020b16d8 {
    unsigned char* tile;    // 0x0
    int field4;             // 0x4
};

struct BlitParams020b16d8 {
    int field00;            // 0x0
    int field04;            // 0x4
    int field08;            // 0x8
    int field0c;            // 0xc
    int field10;            // 0x10
    int field14;            // 0x14
    int field18;            // 0x18
    int field1c;            // 0x1c
    int field20;            // 0x20
    int field24;            // 0x24
};

extern "C" int func_020b10b4(int tx, int ty, int w, int h, int lo, int hi);
extern "C" void func_020b1300(struct BlitParams020b16d8* params);

extern "C" void g1(struct BlitParams020b16d8*);
extern "C" int g2(int, int);

ARM void P5(int a, int b) {
    struct BlitParams020b16d8 s;
    if (a == 0) return;
    s.field00 = g2(a, b);
    s.field04 = g2(b, a);
    s.field08 = g2(a + 1, b);
    s.field0c = g2(a, b + 1);
    s.field10 = g2(a + 2, b);
    s.field14 = g2(a, b + 2);
    g1(&s);
}

ARM void P6(int a, int b) {
    struct BlitParams020b16d8 s;
    int i;
    if (a == 0) return;
    for (i = 0; i < b; i++) {
        s.field04 = i;
        g1(&s);
    }
}

ARM void P7(int a, int b) {
    struct BlitParams020b16d8 s;
    int i;
    s.field00 = a;
    if (a == 0) return;
    for (i = 0; i < b; i++) {
        s.field04 = i;
        g1(&s);
    }
    if (b == 0) return;
    g1(&s);
}

ARM void P8(int a, int b) {
    struct BlitParams020b16d8 s;
    int i;
    int n = a + b;
    s.field00 = a;
    s.field04 = b;
    s.field08 = n;
    s.field0c = n * 2;
    if (a == 0) return;
    n = g2(a, b);
    s.field10 = n;
    if (n < 0) return;
    s.field14 = g2(b, a);
    for (i = 0; i < n; i++) {
        s.field18 = i;
        g1(&s);
    }
}

// USA: func_020b16d8
ARM void BlitTiles020b16d8(struct TileCtx020b16d8* ctx, struct TileSrc020b16d8* src,
                           int x, int y, int n, struct TileRef020b16d8* ref) {
    struct BlitParams020b16d8 params;
    int a;
    int b;
    int c;

    a = ctx->width;
    b = src->set->blob->b0;
    if (ref->tile[1] == 0) {
        return;
    }
    c = x + ref->tile[1];
    if (c < 0) {
        return;
    }
    params.field04 = ref->field4;
    if (y < 0) {
        return;
    }
    params.field00 = func_020b10b4(a, b, c, x, y, n);
    func_020b1300(&params);
}
