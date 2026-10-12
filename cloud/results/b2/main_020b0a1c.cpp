#include <globaldefs.h>

struct ObjEntry020b0a1c {
    unsigned short attr0;
    unsigned short attr1;
    unsigned short attr2;
};

struct ObjList020b0a1c {
    unsigned short count;
    unsigned short pad2;
    ObjEntry020b0a1c* entries;
};

struct ObjAttr020b0a1c {
    union {
        unsigned int attr01;
        struct {
            unsigned short attr0;
            unsigned short attr1;
        } h;
    };
    unsigned short attr2;
    unsigned short pad6;
};

struct Mtx22_020b0a1c {
    int m00;
    int m01;
    int m10;
    int m11;
};

struct Vec2_020b0a1c {
    int x;
    int y;
};

extern const unsigned short data_020e9198[3][4];
extern const unsigned short data_020e9180[3][4];

static inline short SignExt9(short x) {
    return x > 0xff ? x | 0xff00 : x;
}

static inline int ToFix12(short v) {
    return v << 12;
}

static inline short SignExt8(short y) {
    return y > 0x7f ? y | 0xff00 : y;
}

static inline unsigned int ModeWithFlip(unsigned int v, unsigned int mode) {
    return (mode != 0x100 && mode != 0x300) ? mode | (v & 0x30000000) : mode;
}

#define MULVEC22(m, in, out)                                                                      \
    do {                                                                                          \
        Vec2_020b0a1c tmp;                                                                        \
        Vec2_020b0a1c* dst = (out);                                                               \
        if ((out) == (in)) dst = &tmp;                                                            \
        dst->x = (int)((((long long)(m)->m10 * (in)->y) + 0x1000 + ((long long)(m)->m00 * (in)->x)) >> 12); \
        dst->y = (int)((((long long)(m)->m11 * (in)->y) + 0x1000 + ((long long)(m)->m01 * (in)->x)) >> 12); \
        if (dst != (out)) {                                                                       \
            (out)->x = tmp.x;                                                                     \
            (out)->y = tmp.y;                                                                     \
        }                                                                                         \
    } while (0)

// USA: func_020b0a1c
extern "C" ARM unsigned int func_020b0a1c(ObjAttr020b0a1c* dst, unsigned int limit, ObjList020b0a1c* src,
                                          Mtx22_020b0a1c* m, Vec2_020b0a1c* off, unsigned short param,
                                          int dbl) {
    unsigned short i = 0;
    unsigned int count = src->count;
    if (limit >= count) limit = count;
    bool isDbl;
    if (i < limit) {
        do {
            ObjAttr020b0a1c* o = &dst[i];
            ObjEntry020b0a1c* e = &src->entries[i];
            dst[i].h.attr0 = src->entries[i].attr0;
            o->h.attr1 = e->attr1;
            o->attr2 = e->attr2;
            if (m != 0 || off != 0) {
            Vec2_020b0a1c v;
            short sx = (o->attr01 & 0x1ff0000) >> 16;
            if (sx > 0xff) sx |= 0xff00;
            v.x = sx << 12;
            short sy = o->attr01 & 0xff;
            if (sy > 0x7f) sy |= 0xff00;
            v.y = sy << 12;
            if (m) {
                unsigned int mode = ModeWithFlip(o->attr01, o->attr01 & 0x300);
                if (mode == 0x300) {
                    unsigned int sz = o->attr01 & 0xc000c000;
                    int shape = (sz & 0xc000) >> 14;
                    int size = (sz & 0xc0000000) >> 30;
                    v.x += data_020e9198[shape][size] << 11;
                    v.y += data_020e9180[shape][size] << 11;
                }
                unsigned int nm = dbl ? 0x300 : 0x100;
                isDbl = nm == 0x300;
                MULVEC22(m, &v, &v);
                unsigned int a = o->attr01;
                if (nm == 0x100 || nm == 0x300)
                    o->attr01 = (a & 0xc1fffcff) | nm | (param << 25);
                else
                    o->attr01 = (a & 0xc1fffcff) | nm;
                unsigned int sz = o->attr01 & 0xc000c000;
                int shape = (sz & 0xc000) >> 14;
                int size = (sz & 0xc0000000) >> 30;
                int hw = data_020e9198[shape][size] >> 1;
                int hh = data_020e9180[shape][size] >> 1;
                int hwf = hw << 12;
                int hhf = hh << 12;
                v.x += m->m00 * hw - hwf + m->m10 * hh;
                v.y += m->m01 * hw - hhf + m->m11 * hh;
                if (isDbl) {
                    v.x -= hwf;
                    v.y -= hhf;
                }
            }
            if (off) {
                v.x += off->x;
                v.y += off->y;
            }
            o->attr01 = (o->attr01 & 0xfe00ff00) | (((v.y + 0x800) >> 12) & 0xff) | ((((v.x + 0x800) >> 12) & 0x1ff) << 16);
            }
            i++;
        } while (i < limit);
    }
    return limit;
}
