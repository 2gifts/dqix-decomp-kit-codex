#include <globaldefs.h>

struct HashCtx020c08ec {
    unsigned int unk0[5];
    unsigned int totalLo;
    unsigned int totalHi;
    int index;
    unsigned char buffer[0x40];
};

// USA: func_020c0e9c
extern "C" ARM void func_020c0e9c(HashCtx020c08ec* ctx, const unsigned int* data, int len) {
    unsigned int schedule[32];
    unsigned int* w;
    unsigned int a = ctx->unk0[0];
    unsigned int b = ctx->unk0[1];
    unsigned int c = ctx->unk0[2];
    unsigned int d = ctx->unk0[3];
    unsigned int e = ctx->unk0[4];
    unsigned int temp;
    unsigned int v;
    unsigned int x;
    int i;

    do {
        unsigned int mask = 0x00ff00ff;
        w = schedule;
        i = 0;
        do {
            x = *data++;
            v = (((x << 8) | (x >> 24)) & mask) | (((x & mask) >> 8) | ((x & mask) << 24));
            w[16] = v;
            *w++ = v;
            temp = (0x5a827999 + e) + ((a << 5) | (a >> 27)) + v + (d ^ (b & (c ^ d)));
            e = d;
            d = c;
            c = (b >> 2) | (b << 30);
            b = a;
            a = temp;
            i += 4;
        } while (i < 0x40);

        w = schedule;
        i = 0;
        do {
            v = (w[0] ^ w[2]) ^ (w[8] ^ w[13]);
            v = (v << 1) | (v >> 31);
            w[16] = v;
            *w++ = v;
            temp = v + e + 0x5a827999 + ((a << 5) | (a >> 27)) + (d ^ (b & (c ^ d)));
            e = d;
            d = c;
            c = (b >> 2) | (b << 30);
            b = a;
            a = temp;
            i += 4;
        } while (i < 0x10);

        i = 0;
        do {
            v = (w[0] ^ w[2]) ^ (w[8] ^ w[13]);
            v = (v << 1) | (v >> 31);
            w[16] = v;
            *w++ = v;
            temp = v + e + 0x6ed9eba1 + ((a << 5) | (a >> 27)) + (b ^ c ^ d);
            e = d;
            d = c;
            c = (b >> 2) | (b << 30);
            b = a;
            a = temp;
            i++;
            if (i == 12) {
                w = schedule;
            }
        } while (i < 0x14);

        i = 0;
        do {
            v = (w[0] ^ w[2]) ^ (w[8] ^ w[13]);
            v = (v << 1) | (v >> 31);
            w[16] = v;
            *w++ = v;
            temp = v + e + 0x8f1bbcdc + ((a << 5) | (a >> 27)) + (((b | c) & d) | (b & c));
            e = d;
            d = c;
            c = (b >> 2) | (b << 30);
            b = a;
            a = temp;
            i++;
            if (i == 8) {
                w = schedule;
            }
        } while (i < 0x14);

        i = 0;
        do {
            v = (w[0] ^ w[2]) ^ (w[8] ^ w[13]);
            v = (v << 1) | (v >> 31);
            w[16] = v;
            *w++ = v;
            temp = v + e + 0xca62c1d6 + ((a << 5) | (a >> 27)) + (b ^ c ^ d);
            e = d;
            d = c;
            c = (b >> 2) | (b << 30);
            b = a;
            a = temp;
            i++;
            if (i == 4) {
                w = schedule;
            }
        } while (i < 0x14);

        a += ctx->unk0[0];
        b += ctx->unk0[1];
        c += ctx->unk0[2];
        d += ctx->unk0[3];
        e += ctx->unk0[4];
        ctx->unk0[0] = a;
        ctx->unk0[1] = b;
        ctx->unk0[2] = c;
        ctx->unk0[3] = d;
        ctx->unk0[4] = e;
        len -= 0x40;
    } while (len > 0);
}
