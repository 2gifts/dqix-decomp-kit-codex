#include <globaldefs.h>
#include "std_library_functions.h"

extern "C" void* func_0206a3c0(void* ctx, int msgId);
extern "C" int func_02005a94(char* str);

char* FindUnescapedAngleBracket(char* str);
int IsPrefixMatch020d857c(signed char* a, signed char* b);
int StringStartsWithCI020d85dc(const char* str, const char* prefix);
int StringLength(const char* s);

struct TagEntry02069fec {
    const char* name;
    int (*fn)(char**, char*);
};

extern struct TagEntry02069fec data_020e7f84[];

struct NameList02069fec {
    char* p[6];
};
extern struct NameList02069fec data_020e7e74;
extern char data_020f091b[];
extern char data_020f092a[];
extern char data_020f092f[];
struct IdTable02069fec {
    char pad0[0x24];
    unsigned short ids[2];
};
extern struct IdTable02069fec data_020e7e04;

struct Sub02069fec {
    char pad0[0x94a];
    unsigned char depthArray[4];
    char pad1[0x86];
    unsigned char activeFlag;
};

struct Ctx02069fec {
    char pad0[0x1000];
    struct Sub02069fec sub;
};

// USA: func_02069fec
extern "C" ARM void func_02069fec(struct Ctx02069fec* ctx, int msgId, char* outArg) {
    unsigned short endcode;
    char* src;
    if (msgId == 0 || outArg == 0) return;
    char* out = outArg;
    src = (char*)func_0206a3c0(ctx, msgId);
    ctx->sub.activeFlag = 1;
    unsigned short depth = 0;

    for (;;) {
        signed char c = *src;
        if (c == 0) break;

        if ((c == '\\' && src[1] == 'n') || (c == '\r' && src[1] == '\n')) {
            unsigned short code = 0xff18;
            memcpy(out, &code, 2);
            src += 2;
            out += 2;
            continue;
        }
        if (c == '\n') {
            unsigned short code = 0xff18;
            memcpy(out, &code, 2);
            src += 1;
            out += 2;
            continue;
        }
        if (c == '<') {
            char* close = FindUnescapedAngleBracket(src);
            if (close != 0) {
                char* arg = src + 1;
                int matched = 0;
                struct TagEntry02069fec* e = data_020e7f84;
                while (e->name != 0 && e->fn != 0) {
                    if (StringStartsWithCI020d85dc(arg, e->name)) {
                        int len = StringLength(e->name);
                        int n = e->fn(&out, arg + len);
                        out += n;
                        src = close + 1;
                        matched = 1;
                        break;
                    }
                    e++;
                }
                if (matched) continue;

                {
                    struct NameList02069fec names = data_020e7e74;
                    char** np = names.p;
                    while (*np != 0) {
                        if (IsPrefixMatch020d857c((signed char*)src, (signed char*)*np)) {
                            while (*src != '>') { src++; }
                            src++;
                            break;
                        }
                        np++;
                    }
                }

                if (IsPrefixMatch020d857c((signed char*)src, (signed char*)data_020f091b)) {
                    int r7 = 0xe;
                    int id = func_02005a94(src + 0xe);
                    while (src[r7] != '>') {
                        if (src[r7] == 0) return;
                        r7++;
                    }
                    ctx->sub.depthArray[depth] = (unsigned char)id;
                    unsigned short code = depth + 0x4d + 0xff00;
                    memcpy(out, &code, 2);
                    src += r7 + 1;
                    out += 2;
                    depth = (unsigned short)(depth + 1) & 3;
                    continue;
                }
                if (IsPrefixMatch020d857c((signed char*)src, (signed char*)data_020f092a)) {
                    int r7 = 4;
                    int val = func_02005a94(src + 4);
                    while (src[r7] != '>') {
                        if (src[r7] == 0) return;
                        r7++;
                    }
                    unsigned short code = val + 0x334 + 0xfc00;
                    memcpy(out, &code, 2);
                    src += r7 + 1;
                    out += 2;
                    continue;
                }
                if (IsPrefixMatch020d857c((signed char*)src, (signed char*)data_020f092f)) {
                    int r7 = 4;
                    int val = func_02005a94(src + 4);
                    while (src[r7] != '>') {
                        if (src[r7] == 0) return;
                        r7++;
                    }
                    short ids[2];
                    ids[0] = data_020e7e04.ids[0];
                    ids[1] = data_020e7e04.ids[1];
                    unsigned int result = 0xff4b;
                    unsigned short k = 0;
                    while (ids[k] >= 0) {
                        if (val == ids[k]) {
                            result = (unsigned short)(result + k);
                            break;
                        }
                        k++;
                    }
                    unsigned short code = (unsigned short)result;
                    memcpy(out, &code, 2);
                    src += r7 + 1;
                    out += 2;
                    continue;
                }
            }
        }
        *out = *src;
        src++;
        out++;
    }
    endcode = 0xff01;
    memcpy(out, &endcode, 2);
    out += 2;
    *out = 0;
}
