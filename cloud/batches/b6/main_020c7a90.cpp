#include <globaldefs.h>
#include <System/Interrupts.h>
#include <System/ProcessorContext.h>

// KEEP-NAME: the ROM symbol here is the mangled C++ name, not a func_ tag.
// USA: func_020c7a90
extern "C" ARM bool _Z21ChangeContextPriorityP16ProcessorContextj(ProcessorContext *context, unsigned int newPriority)
{
    ProcessorContext *loopEntry = data_021112e0.substruct_24.firstContext;
    ProcessorContext *nodeBefore = NULL;
    int priorState = DisableIRQInterrupts();
    while (loopEntry != NULL && loopEntry != context)
    {
        nodeBefore = loopEntry;
        loopEntry = loopEntry->pNext;
    }
    if (loopEntry == NULL || loopEntry == &data_02111314)
    {
        SetIRQInterruptState(priorState);
        return false;
    }
    if (loopEntry->priority != newPriority)
    {
        if (nodeBefore != NULL)
            nodeBefore->pNext = context->pNext;
        else if (nodeBefore == NULL)
            data_021112e0.substruct_24.firstContext = context->pNext;
        context->priority = newPriority;
        InsertContextIntoGlobalList(context);
        SwitchContext();
    }
    SetIRQInterruptState(priorState);
    return true;
}
