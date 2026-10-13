#include <globaldefs.h>

struct Obj0201b600;
struct EntryList_203dce4;
struct Entry_203dce4;
struct HeadNode02046b24;
struct GameResources;
struct Struct_203cec4;
struct Struct_0203fc5c;
struct Node020406f8;
struct Vec3_020406f8 { int x; int y; int z; };
struct Node02040774;
struct Node020409b0;
struct Obj02040b2c;
struct S02040538;
struct Vec3 { int x; int y; int z; };

extern "C" void* func_02012fe4(void);
extern "C" unsigned char* func_ov017_0218b5b0(void);
extern "C" int rand(void);
extern "C" unsigned int func_020c3260(int phase);
extern "C" unsigned int func_020c32f8(int phase);
extern "C" void Vector3fix_Add(struct Vec3* a, struct Vec3* b, struct Vec3* out);
struct EntryList_203dce4* GetGlobalPtr021075f4();
int GetHeadNodeIdOrMinusOne(struct HeadNode02046b24**);
struct Obj0201b600* FindElemByKeys(struct Obj0201b600*, int, short);
struct Entry_203dce4* GetEntryUnlessFlag0x8000(struct EntryList_203dce4*, int);
void ClearFlag0x1(unsigned int*);
int IsField0Null(void**);
int IsMainBrightnessTransitionActive(struct GameResources*);
int GetMainBrightnessTransitionState(struct GameResources*);
struct S02040538* GetField0xc02040538(struct S02040538*);
void SetActiveChildFields02040774(struct Node02040774*, int, int, int);
void PropagateValueToActiveSlot02040b2c(struct Obj02040b2c*, int);
void SelectVec3FromSources020406f8(struct Vec3_020406f8*, struct Node020406f8*);
void SetOrClearFlag0x8(struct Struct_203cec4*, int);
void SetField40IfPositive(struct Struct_0203fc5c*, float);
void DispatchOnActiveChild020409b0(struct Node020409b0*, int);

// USA: func_020aeb34
extern "C" ARM void TODO_Name_020aeb34(void) {
    void* head;
    void* arr[4];
    unsigned char* base;
    struct Obj0201b600* elem;
    int i;
    struct EntryList_203dce4* list;

    elem = (struct Obj0201b600*)func_02012fe4();
    list = GetGlobalPtr021075f4();
    base = func_ov017_0218b5b0();
    head = *(void**)(base + 0x36fc);
    GetHeadNodeIdOrMinusOne((struct HeadNode02046b24**)head);
    elem = FindElemByKeys(elem, 0, 0x2a);

    if (elem == NULL) return;

    for (i = 0; i < 4; i++) {
        arr[i] = GetEntryUnlessFlag0x8000(list, *(unsigned short*)(base + 0x444a + i * 2));
        if (arr[i] == NULL) return;
        ClearFlag0x1((unsigned int*)arr[i]);
    }

    if (IsField0Null((void**)head) == 0 && IsMainBrightnessTransitionActive((struct GameResources*)base) == 0 &&
        (GetMainBrightnessTransitionState((struct GameResources*)base) == 2 ||
         GetMainBrightnessTransitionState((struct GameResources*)base) == 3)) {
        if (*(short*)((char*)elem + 6) != 0) return;
        for (int j = 0; j < 4; j++) {
            struct S02040538* s = GetField0xc02040538((struct S02040538*)arr[j]);
            SetActiveChildFields02040774((struct Node02040774*)arr[j], *(int*)((char*)s + 0x10), *(int*)((char*)s + 0x14), *(int*)((char*)s + 0x18));
            PropagateValueToActiveSlot02040b2c((struct Obj02040b2c*)arr[j], *(short*)((char*)s + 0x1c));
        }
        return;
    }

    *(int*)(base + 0x4454) += 2 * (rand() % 13 - 6);
    if (*(int*)(base + 0x4454) > 0x2d) *(int*)(base + 0x4454) = 0x2d;
    if (*(int*)(base + 0x4454) < 0x20) *(int*)(base + 0x4454) = 0x20;

    int vecAx = 88006;
    if (*(short*)((char*)elem + 6) <= *(int*)(base + 0x4454)) {
        *(short*)((char*)elem + 6) += 0x6487;
    }
    *(short*)((char*)elem + 6) -= *(int*)(base + 0x4454);

    int angle = *(short*)((char*)elem + 6);

    for (int k = 0; k < 4; k++) {
        int phase;
        struct Vec3 vecB;
        struct Vec3 vecA;
        struct Vec3 out;
        struct Vec3_020406f8 sel;

        angle += 0x1921;
        if (angle >= 0x6487) angle -= 0x6487;
        phase = angle - 0xcc;
        if (phase <= 0) phase += 0x6487;

        vecB.x = (int)(4096.0f * ((float)func_020c3260(phase) / 4294967296.0f)) * 2;
        vecB.y = 0;
        vecB.z = (int)(4096.0f * ((float)func_020c32f8(phase) / 4294967296.0f)) * 2;

        vecA.x = vecAx;
        void* entry = arr[k];
        SelectVec3FromSources020406f8(&sel, (struct Node020406f8*)entry);
        vecA.z = -0x970;
        vecA.y = *(int*)((char*)&sel + 4);

        Vector3fix_Add(&vecA, &vecB, &out);

        if (*(void**)((char*)entry + 0x14) != NULL) SetOrClearFlag0x8(*(struct Struct_203cec4**)((char*)entry + 0x14), 1);
        if (*(void**)((char*)entry + 0x14) != NULL) SetField40IfPositive(*(struct Struct_0203fc5c**)((char*)entry + 0x14), 0.8f);
        DispatchOnActiveChild020409b0((struct Node020409b0*)entry, (int)&out);
        SetActiveChildFields02040774((struct Node02040774*)entry, out.x, out.y, out.z);
    }
}
