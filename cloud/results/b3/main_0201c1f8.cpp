#include <globaldefs.h>
#include "Resource/Script.h"

struct Queue_0201c4d4;
struct ScriptRecord {
    unsigned short id_, value_;
    unsigned char history_;
    char name_[8], text_[28];
    unsigned char category_ : 3;
    unsigned char flag3_ : 1;
    unsigned char flag4_ : 1;
    unsigned char flag5_ : 1;
    unsigned char flag6_ : 1;
    unsigned char flag7_ : 1;
    unsigned char mode_;
};
struct ScriptQueues { void* fields_[2]; Queue_0201c4d4* queue_; };
extern ScriptQueues data_020fdc14 __attribute__((aligned(4)));
extern "C" void func_0201c4d4(Queue_0201c4d4*, ScriptRecord*);

// USA: func_0201c1f8
extern "C" ARM int func_0201c1f8(Script::Parameter* params) {
    ScriptRecord record;
    record.id_ = 0;
    record.value_ = 0;
    record.name_[0] = 0;
    record.history_ = 0;
    record.category_ = 0;
    record.flag4_ = 1;
    record.flag3_ = 0;
    record.flag5_ = 0;
    record.flag6_ = 0;
    record.flag7_ = 0;
    record.id_ = params[0].ToInt();
    record.value_ = params[1].ToInt();
    params[2].ToString();
    record.category_ = (unsigned char)params[3].ToInt();
    const char* name = params[4].ToString();
    const char* text = params[5].ToString();
    record.mode_ = params[6].ToInt();
    record.flag4_ = (unsigned char)params[7].ToInt();
    record.flag3_ = (unsigned char)params[8].ToInt();
    record.flag5_ = (unsigned char)params[9].ToInt();
    record.flag6_ = (unsigned char)params[10].ToInt();
    record.flag7_ = (unsigned char)params[11].ToInt();
    if (!text) return 0;
    memcpy(record.name_, name, sizeof(record.name_));
    record.name_[7] = 0;
    memcpy(record.text_, text, sizeof(record.text_));
    Queue_0201c4d4* queue = data_020fdc14.queue_;
    record.text_[27] = 0;
    func_0201c4d4(queue, &record);
    return 1;
}
