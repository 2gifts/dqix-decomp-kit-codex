#include <globaldefs.h>

struct GameState;
struct SafeAllocator;

struct MonsterKey0201a300 {
    unsigned short key;
};

struct GrottoWork0201a300 {
    char head[0x40];
    char buf[0x100];
};

extern "C" {
GameState* _ZN9GameState11GetInstanceEv();
void srand(unsigned int seed);
void* memcpy(void* dst, const void* src, unsigned int n);
int abs(int x);
}

// USA: func_0201a300
extern "C" ARM int func_0201a300(char* zone, int floor, void* monster, int a, int b) {
    GameState* gs = _ZN9GameState11GetInstanceEv();
    GrottoWork0201a300 work;
    if (monster == 0) {
        return 1;
    }
    MonsterKey0201a300* m = (MonsterKey0201a300*)monster;
    srand(m->key % 20);
    char** src = (char**)b;
    if (src == 0) {
        src = (char**)(zone + 0x25b4);
    }
    memcpy(work.buf, *src, 0x100);
    int i = 0;
    int j = i;
    int flag = 1;
    return j;
}
