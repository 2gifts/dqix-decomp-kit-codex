#include <globaldefs.h>
#include "System/Interrupts.h"
#include "System/ProcessorContext.h"

// KEEP-NAME: the ROM symbol here is the mangled C++ name, not a func_ tag.
// USA: func_020c79bc
extern "C" ARM void _Z28CycleCurrentPriorityContextsv() {
    ProcessorContext *prevOfActive = NULL;
    ProcessorContext *lastOfPriority = NULL;
    int count = 0;
    int savedInterruptState;
    ProcessorContext *active = data_021112e0.substruct_24.activeContext;
    savedInterruptState = DisableIRQInterrupts();
    ProcessorContext *prev = NULL;
    ProcessorContext *context = data_021112e0.substruct_24.firstContext;
    if (context != NULL) {
        unsigned int activePriority = active->priority;
        do {
            if (context == active) {
                prevOfActive = prev;
            }
            if (activePriority == context->priority) {
                lastOfPriority = context;
                count++;
            }
            prev = context;
            context = context->pNext;
        } while (context != NULL);
    }
    if (count <= 1 || lastOfPriority == active) {
        SetIRQInterruptState(savedInterruptState);
        return;
    }
    if (prevOfActive != NULL) {
        prevOfActive->pNext = active->pNext;
    }
    if (prevOfActive == NULL) {
        data_021112e0.substruct_24.firstContext = active->pNext;
    }
    active->pNext = lastOfPriority->pNext;
    lastOfPriority->pNext = active;
    SwitchContext();
    SetIRQInterruptState(savedInterruptState);
}
