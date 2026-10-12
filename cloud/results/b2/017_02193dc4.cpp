#include <globaldefs.h>
#include "GameState/GameState.h"
#include "Graphics/Vector.h"
#include "Resource/GameResources.h"
#include "World/Object3D.h"

struct GeometryTriangle {
    Vector3fix vertices[3];
    Vector3fix normal;
};

struct FourHalfwords02033f7c {
    unsigned short f0;
    unsigned short f2;
    unsigned short f4;
    unsigned short f6;
};

struct PolygonHeader {
    int type;
};

struct PolygonData {
    PolygonHeader* header;
    unsigned char pad_4[0x10];
    FourHalfwords02033f7c* materials;
};

struct GeometryData {
    int unused;
    PolygonData* polygons;
};

struct TerrainNode {
    unsigned char pad_0[0x24];
    GeometryData* geometry;
};

struct Zone02193dc4 {
    unsigned char pad_0[0x824];
    TerrainNode** terrain;
    unsigned char pad_828[0x4];
    int count82c;
};

struct TerrainGrid02193dc4 {
    short cells[96][12];
    int unused900;
    int scale;
    Vector3i position;
    int unused914;
    int angle;
    int unused91c;
    unsigned char materialIndex[96];
};

struct Probe02193dc4 {
    Vector3i top;
    Vector3i bottom;
};

class Actor02193dc4 : public Object3D {
public:
    short fieldAC;
};

extern "C" void* func_02012fe4(void);
extern "C" void func_02031188(GeometryTriangle* dst, short* src);
extern "C" long long func_020c32f8(int);
extern "C" long long func_020c3260(int);
extern "C" float func_0200c700(long long);
extern "C" int func_020312bc(GeometryTriangle* triangles, int count, Vector3fix* start, Vector3fix* end, Vector3fix* point);
extern "C" int func_02017d90(Zone02193dc4* zone, int index, TerrainGrid02193dc4* grid, Probe02193dc4 probe, Actor02193dc4* self);
void ClearFlags0x40And0x80At0xe0(struct ByteFlags02033f2c* p);
void CopyFourHalfwordsToField0x114(void* obj, FourHalfwords02033f7c* src);

// USA: func_ov017_02193dc4
extern "C" ARM int func_ov017_02193dc4(Actor02193dc4* self) {
    TerrainGrid02193dc4 grid;
    GeometryTriangle samples[96];
    Zone02193dc4* zone;
    int total;
    int i;

    GameState::GetInstance();
    func_ov017_0218b5b0();
    zone = (Zone02193dc4*)func_02012fe4();
    Probe02193dc4 probe;
    Vector3i pos = self->position_;
    total = 0;
    probe.top = pos;
    probe.bottom = pos;
    probe.top.x += self->GetRadius() / 2;
    probe.top.y += self->GetHeight();
    probe.top.z += self->GetRadius() / 2;
    probe.bottom.x -= self->GetRadius() / 2;
    probe.bottom.y -= 0x28000;
    probe.bottom.z -= self->GetRadius() / 2;
    ClearFlags0x40And0x80At0xe0((ByteFlags02033f2c*)self);
    for (i = 0; i < zone->count82c; i++) {
        int count = func_02017d90(zone, i, &grid, probe, self);
        if (count > 0) {
            total += count;
            for (int k = 0; k < count; k++) {
                func_02031188(&samples[k], grid.cells[k]);
                Vector3fixMultiplyScalar(&samples[k].vertices[0], grid.scale << 12, &samples[k].vertices[0]);
                Vector3fixMultiplyScalar(&samples[k].vertices[1], grid.scale << 12, &samples[k].vertices[1]);
                Vector3fixMultiplyScalar(&samples[k].vertices[2], grid.scale << 12, &samples[k].vertices[2]);
            }
            Vector3i local = pos;
            Matrix3x3 rotation;
            Vector3i top;
            Vector3i bottom;
            Vector3i hit;
            Vector3fix_Subtract(&local, &grid.position, &local);
            int rotationAngle = -(grid.angle - 25735);
            int cosine = (int)(4096.0f * (func_0200c700(func_020c32f8(rotationAngle)) / 4294967296.0f));
            int sine = (int)(4096.0f * (func_0200c700(func_020c3260(rotationAngle)) / 4294967296.0f));
            Mat3x3_WriteRotationY(&rotation, sine, cosine);
            Mat3x3_ApplyToVector(&local, &rotation, &local);
            top = local;
            top.y += 0x800;
            bottom = local;
            bottom.y -= 0x28000;
            int idx = func_020312bc(samples, count, &top, &bottom, &hit);
            if (idx != -1) {
                hit.y += 0x199;
                Vector3fix_Add(&hit, &grid.position, &hit);
                TerrainNode* node = zone->terrain[i];
                GeometryData* geometry;
                PolygonData* polygons;
                if (node != NULL && (geometry = node->geometry) != NULL && (polygons = geometry->polygons) != NULL) {
                    if (polygons->header->type == 3) {
                        FourHalfwords02033f7c material = polygons->materials[grid.materialIndex[idx]];
                        CopyFourHalfwordsToField0x114(self, &material);
                    }
                }
            }
        }
    }
    self->fieldAC = 0;
    return total;
}
