#include <globaldefs.h>
#include <GameState/GameState.h>
#include <Util/Random.h>
struct ChoiceA700 { char pad[0x38]; unsigned char toggle; };
struct ValuesA700 { char pad[0x18]; unsigned short values[6]; };
struct CombatantA700 { char pad[0x138]; ChoiceA700* choices; char pad2[0xc]; ValuesA700* values; };
struct SelectorA700 { Random* random; };
GameObject* GetCombatantWithFlag0x400(GameState*,int);
extern "C" int func_0208a03c(SelectorA700*, int, int, int*, int);
extern "C" int func_ov000_02154a04(Random*,int,int,int);
// USA: func_0208a700
extern "C" ARM int func_0208a700(SelectorA700* self,int id,int* out,int arg) {
    CombatantA700* c=(CombatantA700*)GetCombatantWithFlag0x400(GameState::GetInstance(),id);
    ChoiceA700* choices=c->choices;
    ValuesA700* values=c->values;
    unsigned char pass=0;
    choices->toggle &= 1;
    for (;pass<2;pass++) {
        unsigned char min=0;
        unsigned char max=0;
        if (choices->toggle) { min=1; max=5; }
        unsigned char count=max-min+1;
        unsigned char choice = NextRandomMax(self->random,count);
        choices->toggle++;
        unsigned char attempt=0;
        choices->toggle &= 1;
        for (;attempt<count;attempt++) {
            unsigned char index=min+choice;
            if (func_0208a03c(self,index,id,out,arg)) return values->values[index];
            choice=(unsigned char)(choice+1) % count;
        }
    }
    *out=0;
    *out=func_ov000_02154a04(self->random,id,2,arg);
    return 2;
}
