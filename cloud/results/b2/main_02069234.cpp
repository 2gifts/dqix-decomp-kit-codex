#include <globaldefs.h>
#include "std_library_functions.h"

struct GameState;
struct TagContext02069234;

struct StatusEffectEntry02069234 {
    void* f0;
    char* fmt;
    unsigned int kind : 4;
    unsigned int : 28;
    char pad2[0xc];
    short duration;
};

struct StatusTable02069234 {
    char pad[0x294];
    StatusEffectEntry02069234 effect;
};

struct Combatant02069234 {
    char unk[0x150];
    StatusTable02069234* statusTable;
};

struct TagContext02069234 {
    void* slots0[2];
    void* slots8[2];
    void* slots10[2];
    void* slots18[2];
    void* slots20[2];
    void* unk28;
    void* unk2c;
    char pad30[0x37c];
    char text[0x14d4];
    signed char combatantId;
};

extern "C" Combatant02069234* _Z25GetCombatantWithFlag0x100P9GameStatei(GameState* gameState, int combatantId);
extern "C" GameState* _ZN9GameState11GetInstanceEv();

char* FindUnescapedAngleBracket(char* str);
int IsPrefixMatch020d857c(signed char* a, signed char* b);
int StringLength(const char* s);
extern "C" int func_02005a94(void* p);
extern "C" int func_02001aec(void* a, void* b, int n);
extern "C" char* func_020e4958(char** dst, void* val, int a2, int a3, int a4, int a5);

typedef void (*TagHandler02069234)(char* tag, char** dst, TagContext02069234* sb, int index);

struct PrefixEntry02069234 {
    char* prefix;
    TagHandler02069234 handler;
};

struct OptEntry02069234 {
    char* key;
    signed char opcode;
    signed char value;
    char pad[2];
};

struct OptTable02069234 {
    OptEntry02069234 e[16];
};

extern PrefixEntry02069234 data_020e7e8c[];
extern OptTable02069234 data_020e7f04;
extern char data_020f0904[4];
extern char data_020f0909[6];
extern char data_020f0910[];

// USA: func_02069234
extern "C" ARM void func_02069234(TagContext02069234* sb, char* str, char* dst) {
    char* cursor;
    if (str == 0 || dst == 0) {
        return;
    }
    cursor = dst;

    for (;;) {
        char c = *str;
        if (c == 0) {
            break;
        }
        if (c == '<') {
            char* gt = FindUnescapedAngleBracket(str);
            if (gt == 0) {
                goto copy_char;
            }
            int handled = 0;
            char* tag = str + 1;
            struct PrefixEntry02069234* rec = data_020e7e8c;
            for (; rec->prefix != 0; rec++) {
                if (!IsPrefixMatch020d857c((signed char*)tag, (signed char*)rec->prefix)) {
                    continue;
                }
                if (rec->handler != 0) {
                    int index = 0;
                    int len = StringLength(rec->prefix);
                    signed char d = tag[len];
                    if (d >= '1' && d <= '9') {
                        index = func_02005a94(tag + len) - 1;
                    }
                    rec->handler(tag, &cursor, sb, index);
                }
                str = gt + 1;
                handled = 1;
                break;
            }
            if (handled) {
                continue;
            }

            int local14;
            int local10;
            int localC;
            int localSl;
            localSl = localC = local14 = -1;
            void* val;
            local10 = 0;
            val = 0;
            struct OptTable02069234 opts = data_020e7f04;
            handled = 0;
            struct OptEntry02069234* opt = opts.e;
            for (;;) {
                char* key = opt->key;
                if (key == 0) {
                    break;
                }
                int len = StringLength(key);
                if (func_02001aec(tag, key, len) == 0) {
                    handled = 1;
                    tag += len;
                    int index = 0;
                    if (opt->value < 0) {
                        signed char d = *tag;
                        if (d >= '1' && d <= '9') {
                            index = func_02005a94(tag) - 1;
                        }
                    }
                    switch (opt->opcode) {
                        case 0: local14 = opt->value; break;
                        case 1: local10 = opt->value; break;
                        case 2: localC = opt->value; break;
                        case 3: localSl = opt->value; break;
                        case 4: val = sb->slots18[index]; break;
                        case 5: val = sb->slots20[index]; break;
                        case 6: val = sb->slots0[index]; break;
                        case 7: val = sb->slots10[index]; break;
                        case 8: val = sb->slots8[index]; break;
                        case 9: val = sb->unk2c; break;
                    }
                }
                opt++;
            }

            if (val != 0) {
                cursor = func_020e4958(&cursor, val, local14, local10, localC, localSl);
            }
            if (!handled) {
                goto copy_char;
            }
            str = gt + 1;
            continue;
        }
        if (c == '[') {
            if (func_02001aec(str, data_020f0904, 4) == 0) {
                str += 4;
                GameState* gameState = _ZN9GameState11GetInstanceEv();
                Combatant02069234* combatant = _Z25GetCombatantWithFlag0x100P9GameStatei(gameState, sb->combatantId);
                if (combatant != 0) {
                    StatusEffectEntry02069234* eff = &combatant->statusTable->effect;
                    if (eff != 0) {
                        if (eff->duration > 0) {
                            if (eff->kind == 0) {
                                cursor += sprintf(cursor, eff->fmt);
                            }
                        }
                    }
                }
                goto skip_bracket;
            }
            if (func_02001aec(str, data_020f0909, 6) == 0) {
                str += 6;
                cursor += sprintf(cursor, sb->text);
                goto skip_bracket;
            }
            *cursor = c;
            cursor++;
            str++;
            continue;
        }
        goto copy_char;
    skip_bracket:
        while (*str != ']' && *str != 0) {
            str++;
        }
        if (*str == 0) {
            cursor += sprintf(cursor, data_020f0910);
            break;
        }
        str++;
        continue;
    copy_char:
        *cursor = *str++;
        cursor++;
    }
    *cursor = 0;
}
