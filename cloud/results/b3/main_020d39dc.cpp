#include <globaldefs.h>
#include "std_library_functions.h"

struct Variant02030b0c { int tag; int val; };
int GetIntFromVariant02030b0c(struct Variant02030b0c* p);

struct Struct02030b7c;
void* GetField4IfField0Zero(struct Struct02030b7c* s);

struct Entry020d3cf0;
void AppendNamedEntry020d3cf0(struct Container020d3cf0* c, struct Entry020d3cf0* src);
struct Container020d3cf0;
extern struct Container020d3cf0* data_021142c4;

struct EntryB10Bits_020d39dc { unsigned char f0:3, f1:1, f2:3, f3:1; };
struct EntryWordBits_020d39dc { unsigned int f0:10, f1:10, f2:10, resv:2; };

struct EntryBuild_020d39dc {
    unsigned char name[16];
    struct EntryB10Bits_020d39dc bits10;
    unsigned char b11;
    unsigned char b12;
    unsigned char b13;
    struct EntryWordBits_020d39dc w14;
    struct EntryWordBits_020d39dc w18;
    struct EntryWordBits_020d39dc w1c;
    unsigned short h20;
    unsigned short h22;
};

// USA: func_020d39dc  (semantic: BuildAndAppendVariantEntry_020d39dc)
extern "C" ARM int func_020d39dc(struct Variant02030b0c* args) {
    struct EntryBuild_020d39dc entry;

    entry.bits10.f0 = GetIntFromVariant02030b0c(&args[0]);
    entry.b11 = GetIntFromVariant02030b0c(&args[1]);
    strcpy((char*)entry.name, (char*)GetField4IfField0Zero((struct Struct02030b7c*)&args[2]));
    entry.bits10.f1 = GetIntFromVariant02030b0c(&args[3]);
    entry.bits10.f2 = GetIntFromVariant02030b0c(&args[4]);
    entry.b12 = GetIntFromVariant02030b0c(&args[5]);
    entry.bits10.f3 = GetIntFromVariant02030b0c(&args[6]);

    entry.w14.f0 = GetIntFromVariant02030b0c(&args[7]);
    entry.w14.f2 = GetIntFromVariant02030b0c(&args[8]);
    entry.w14.f1 = GetIntFromVariant02030b0c(&args[9]);

    entry.w18.f0 = GetIntFromVariant02030b0c(&args[10]);
    entry.w18.f1 = GetIntFromVariant02030b0c(&args[11]);
    entry.w18.f2 = GetIntFromVariant02030b0c(&args[12]);

    entry.w1c.f0 = GetIntFromVariant02030b0c(&args[13]);
    entry.w1c.f1 = GetIntFromVariant02030b0c(&args[14]);
    entry.w1c.f2 = GetIntFromVariant02030b0c(&args[15]);

    entry.b13 = GetIntFromVariant02030b0c(&args[16]);
    entry.h20 = GetIntFromVariant02030b0c(&args[17]);
    entry.h22 = GetIntFromVariant02030b0c(&args[18]);

    AppendNamedEntry020d3cf0(data_021142c4, (struct Entry020d3cf0*)&entry);
    return 1;
}
