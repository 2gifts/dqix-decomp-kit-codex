#include <globaldefs.h>
#include "System/Matrix.h"
#include "Graphics/Vector.h"

struct Vec3Target0204c608 {
    int x;
    int y;
    int z;
};

struct Vec0203109c {
    int x;
    int y;
    int z;
};

struct Box0203109c {
    Vec0203109c lo;
    Vec0203109c hi;
};

struct GridHeader0204c030 {
    int type;
    int shift;
    short s8;
    short sa;
    short sc;
    short se;
    short s10;
    short s12;
    int pad14;
    short s18;
    short s1a;
    unsigned int cols;
    unsigned int rows;
};

struct Cell0204c030 {
    Vector3fix16 v[4];
    unsigned char i0 : 4;
    unsigned char i1 : 4;
    unsigned char i2 : 4;
    unsigned char i3 : 4;
    unsigned char i4 : 4;
    unsigned char i5 : 4;
    unsigned char mark : 1;
    unsigned char tag : 7;
};

struct Grid0204c030 {
    GridHeader0204c030* hdr;
    Cell0204c030* cells;
    unsigned char* counts;
    unsigned short* starts;
    unsigned short* items;
    int pad14;
    Box0203109c bounds;
    Vector3i min;
    Vector3i max;
    unsigned int limit;
};

struct Quad0204c030 {
    Vector3fix16 v[4];
};

struct Result0204c030 {
    Quad0204c030 quads[0x60];
    int count;
    int mask;
    Vector3i min;
    Vector3i max;
};

struct Idx7 {
    int v[7];
};

extern Idx7 data_020e7bb8;
extern int data_02107918;

void SetVec3At0x0(struct Vec3Target0204c608*, int, int, int);
extern "C" bool func_0203109c(Box0203109c* a, Box0203109c* b);

// USA: func_0204c030
ARM int func_0204c030(Grid0204c030* g, Result0204c030* out, Box0203109c box) {
    Vector3fix_Subtract((Vector3fix*)&box.hi, (Vector3fix*)&g->min, (Vector3fix*)&box.hi);
    Vector3fix_Subtract((Vector3fix*)&box.lo, (Vector3fix*)&g->min, (Vector3fix*)&box.lo);
    if (!func_0203109c(&box, &g->bounds)) {
        return 0;
    }
    box.hi.x >>= g->hdr->shift;
    box.hi.y >>= g->hdr->shift;
    box.hi.z >>= g->hdr->shift;
    box.lo.x >>= g->hdr->shift;
    box.lo.y >>= g->hdr->shift;
    box.lo.z >>= g->hdr->shift;
    int midx = (box.lo.x + box.hi.x) / 2;
    int midz = (box.lo.z + box.hi.z) / 2;

    Idx7 idx = data_020e7bb8;
    int n = 0;
    if (g->limit <= 7) {
        n = g->limit;
    } else {
        int gz = (int)((float)fix32_Divide(midz - g->hdr->sc, g->hdr->s18) / 4096.0f);
        if (gz >= 0 && gz < g->hdr->rows) {
            int ok = 0;
            if (gz % 2 == 0) {
                int gx = (int)((float)fix32_Divide(midx - g->hdr->s8, g->hdr->s18) / 4096.0f);
                if (gx >= 0 && gx < g->hdr->cols) {
                    ok = 1;
                }
                if (ok) {
                    unsigned int cols = g->hdr->cols;
                    int c = gz * cols + gx + gz / 2;
                    idx.v[n++] = c;
                    if (gz > 0) {
                        idx.v[n++] = c - g->hdr->cols;
                        idx.v[n++] = c - g->hdr->cols - 1;
                    }
                    if (gz < g->hdr->rows - 1) {
                        idx.v[n++] = c + g->hdr->cols;
                        idx.v[n++] = c + g->hdr->cols + 1;
                    }
                    if (gx > 0) {
                        idx.v[n++] = c - 1;
                    }
                    if (gx < cols - 1) {
                        idx.v[n++] = c + 1;
                    }
                }
            } else {
                int gx = (int)((float)fix32_Divide(midx - g->hdr->s8 + g->hdr->s18 / 2, g->hdr->s18) / 4096.0f);
                if (gx >= 0 && gx < g->hdr->cols + 1) {
                    ok = 1;
                }
                if (ok) {
                    unsigned int cols = g->hdr->cols;
                    int c = gz * cols + gx + gz / 2;
                    idx.v[n++] = c;
                    if (gz > 0) {
                        if (gx > 0) {
                            idx.v[n++] = c - cols - 1;
                        }
                        if (gx < cols) {
                            idx.v[n++] = c - cols;
                        }
                    }
                    if (gz < g->hdr->rows - 1) {
                        if (gx > 0) {
                            idx.v[n++] = c + cols;
                        }
                        if (gx < cols) {
                            idx.v[n++] = c + cols + 1;
                        }
                    }
                    if (gx > 0) {
                        idx.v[n++] = c - 1;
                    }
                    if (gx < cols) {
                        idx.v[n++] = c + 1;
                    }
                }
            }
        }
    }

    int held = 0;
    int count = 0;
    Cell0204c030* heldCells[0xc0];
    for (int i = 0; i < n; i++) {
        int cell = idx.v[i];
        int num = g->counts[cell];
        int start = g->starts[cell];
        for (int j = 0; j < num; j++) {
            Cell0204c030* e = &g->cells[g->items[start + j]];
            if (e->mark) {
                continue;
            }
            e->mark = 1;
            heldCells[held++] = e;
            if (held == 0xc0) {
                break;
            }
            Box0203109c t;
            SetVec3At0x0((Vec3Target0204c608*)&t.hi, ((short*)e)[e->i0], ((short*)e)[e->i1], ((short*)e)[e->i2]);
            SetVec3At0x0((Vec3Target0204c608*)&t.lo, ((short*)e)[e->i3], ((short*)e)[e->i4], ((short*)e)[e->i5]);
            Box0203109c u = t;
            Box0203109c w = u;
            if (func_0203109c(&w, &box)) {
                Quad0204c030* o = &out->quads[count];
                Vector3fix16Copy(&o->v[0], &e->v[0]);
                Vector3fix16Copy(&o->v[1], &e->v[1]);
                Vector3fix16Copy(&o->v[2], &e->v[2]);
                Vector3fix16Copy(&o->v[3], &e->v[3]);
                if (g->hdr->type == 3) {
                    ((unsigned char*)out)[0x920 + count] = e->tag;
                }
                count++;
                out->count = count;
                if (count == 0x60) {
                    break;
                }
            }
        }
        if (count == 0x60) {
            break;
        }
    }
    for (int k = 0; k < held; k++) {
        heldCells[k]->mark = 0;
    }
    if (data_02107918 != count) {
        data_02107918 = count;
    }
    out->mask = 1 << g->hdr->shift;
    out->min = g->min;
    out->max = g->max;
    return count;
}
