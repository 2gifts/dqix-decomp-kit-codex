#include <globaldefs.h>
#include <std_library_functions.h>

struct TextContext {
    char pad_0[0x70];
    int lineCount;
};

struct GlyphEntry {
    char* text;
    signed char width;
    signed char size : 6;
    signed char flags : 2;
    char pad_6[2];
};

struct TagEntry {
    const char* name;
    int (*handler)(const char*, int*, int, int);
};

int GetWordFromTable02042648(int);
bool IsHighByteFF02044494(void*, void*);
int Dispatch02044420(void*, void*);
int FindEntryIndexByKey020424e4(int, int);
char* FindUnescapedAngleBracket(char*);
int StringStartsWithCI020d85dc(const char*, const char*);
int StringLength(const char*);
void* GetBoundedEntry020425b4(int, int);
int LookupKeyValue020425e4(int, int, int);
extern "C" int func_02001aec(void*, const void*, int);

extern char data_020f0979[];
extern char data_020f097c[];
extern char data_020f0981[];
extern char data_020f08dc[];
extern char data_020f0988[];
extern TagEntry data_020e7ec4[];
extern char data_020f098c[];
extern char data_020f0990[];
extern char data_020e7e06[];
extern char data_020f0992[];

// USA: func_0206b974
extern "C" ARM void TODO_Name_0206b974(TextContext* ctx, int countLines, char* src, char* dst, int limit, int baseWidth, int wide, int fontTable) {
    unsigned short cmd;
    char* markSrc = 0;
    char* markDst = 0;
    int markWidth = 0;
    int lineExtra = 0;
    int prev;
    int measuring;
    int width;
    int needMark;
    int isDash;
    int glyphIndex;
    int defaultWidth;
    int step;
    int advance;
    char c;

    char* found1 = strstr(src, data_020f0979);
    char* found2 = strstr(src, data_020f097c);
    if (found1 != 0 || found2 != 0) {
        lineExtra = 2;
    }
    width = baseWidth;
    prev = 0xff;
    measuring = 1;
    needMark = 0;
    defaultWidth = (unsigned char)GetWordFromTable02042648(fontTable);
    for (;;) {
        c = *src;
        if (c == 0) {
            break;
        }
        memcpy(&cmd, src, 2);
        if (IsHighByteFF02044494(ctx, &cmd) && cmd != 0xff19) {
            int n = Dispatch02044420(ctx, &cmd) + 1;
            memcpy(dst, src, n * 2);
            src += n * 2;
            dst += n * 2;
            if (cmd == 0xff18) {
                lineExtra = 0;
                width = baseWidth;
                prev = 0xff;
                markSrc = src;
                markDst = dst;
                markWidth = baseWidth;
                needMark = lineExtra;
            }
            continue;
        }
        if (c == '\\' && src[1] == 'n') {
            lineExtra = 0;
            width = baseWidth;
            prev = 0xff;
            memcpy(dst, src, 2);
            src += 2;
            dst += 2;
            markSrc = src;
            markDst = dst;
            markWidth = width;
            needMark = 0;
            continue;
        }
        if (c == 0xa) {
            lineExtra = 0;
            width = baseWidth;
            prev = 0xff;
            memcpy(dst, src, 1);
            src += 1;
            dst += 1;
            markSrc = src;
            markDst = dst;
            markWidth = width;
            needMark = 0;
            continue;
        }
        if (func_02001aec(src, data_020f0981, 6) == 0) {
            lineExtra = 0;
            width = baseWidth;
            prev = 0xff;
            memcpy(dst, src, 6);
            src += 6;
            dst += 6;
            markSrc = src;
            markDst = dst;
            markWidth = width;
            needMark = 0;
            continue;
        }
        if (func_02001aec(src, data_020f08dc, 2) == 0) {
            char* end = strstr(src, data_020f0988);
            if (end != 0) {
                width = 0;
                int n = end - src;
                memcpy(dst, src, n + 3);
                src += n + 3;
                dst += n + 3;
                continue;
            }
        }
        glyphIndex = FindEntryIndexByKey020424e4((int)src, fontTable);
        if (cmd == 0xff19) {
            glyphIndex = -1;
        }
        if (c == '<' && glyphIndex < 0) {
            char* close = FindUnescapedAngleBracket(src);
            if (close != 0) {
                char* name = src + 1;
                TagEntry* e = data_020e7ec4;
                for (; e->name != 0 && e->handler != 0; e++) {
                    if (StringStartsWithCI020d85dc(name, e->name) != 0) {
                        int r = e->handler(name + StringLength(e->name), &width, fontTable, baseWidth);
                        switch (r) {
                        case 1:
                            measuring = 0;
                            break;
                        case 2:
                            measuring = 1;
                            break;
                        }
                        break;
                    }
                }
                int n = close - src;
                memcpy(dst, src, n + 1);
                src += n + 1;
                dst += n + 1;
                continue;
            }
        }
        isDash = 0;
        advance = defaultWidth + 1;
        step = 1;
        if (cmd == 0xff19) {
            needMark = step;
            step = 2;
        }
        if (c == ' ') {
            needMark = 0;
        }
        if (func_02001aec(src, data_020f098c, 3) == 0) {
            glyphIndex = -1;
            advance = 0;
            src += 3;
            step = advance;
            needMark = 1;
        }
        if (glyphIndex >= 0) {
            GlyphEntry* g = (GlyphEntry*)GetBoundedEntry020425b4(glyphIndex, fontTable);
            step = g->size;
            int key = glyphIndex & 0xff;
            int pair = LookupKeyValue020425e4(prev, key, fontTable);
            prev = key;
            advance = pair + g->width;
            isDash = *g->text == '-';
            if (isDash) {
                needMark = 0;
            }
            if (markSrc != 0 && markDst != 0 && measuring != 0 && limit + lineExtra < width + advance) {
                width = (width - markWidth) + baseWidth;
                lineExtra = 0;
                int len = src - markSrc;
                int cleared = dst - markDst;
                dst = markDst;
                memset(markDst, lineExtra, cleared);
                if (needMark) {
                    memcpy(markDst, data_020f0990, 1);
                    dst = markDst + 1;
                }
                if (wide != 0) {
                    memcpy(dst, data_020e7e06, 2);
                    dst += 2;
                } else {
                    memcpy(dst, data_020f0992, 1);
                    dst += 1;
                }
                memcpy(dst, markSrc, len);
                dst += len;
                if (countLines != 0) {
                    ctx->lineCount = ctx->lineCount + 1;
                }
                needMark = 0;
            }
            advance += 1;
        }
        if (measuring != 0) {
            width += advance;
        }
        memcpy(dst, src, step);
        src += step;
        dst += step;
        if (glyphIndex < 0 || isDash) {
            markSrc = src;
            markDst = dst;
            markWidth = width;
        }
    }
    *dst = c;
}
