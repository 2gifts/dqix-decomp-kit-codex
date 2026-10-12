#include <globaldefs.h>

#include "Combat/ResourceScriptEntry.h"

void CopyFiveFields020823e4(ResourceScriptFields *dst, ResourceScriptFields *src);

// USA: func_02082064
extern "C" ARM int func_02082064(Script::Parameter *params) {
    ResourceScriptFields fields;
    int n = data_02108ee8.field10 + 1;
    fields.value4Low = n;
    data_02108ee8.field10 = n;
    fields.value0 = (params++)->ToInt();
    if (data_02108ee8.field8 < (unsigned int) data_02108ee8.field10 && fields.value0 <= (unsigned int) data_02108ee8.field4) {
        fields.value8.low = (params++)->ToInt();
        fields.value8.high = (params++)->ToInt();
        fields.value8.middle = (params++)->ToInt();
        fields.valueC.low = (params++)->ToInt();
        fields.valueC.middle = (params++)->ToInt();
        fields.value10.low = (params++)->ToInt();
        fields.valueC.high = (params++)->ToInt();
        fields.value10.middle = (params++)->ToInt();
        fields.value10.high = (params++)->ToInt();
        fields.value4High = (params++)->ToInt();
        CopyFiveFields020823e4(&((ResourceScriptEntry *) data_02108ee8.fieldC)->final, &fields);
        data_02108ee8.field0 = 1;
    }
    if (data_02108ee8.field10 == data_02108ee8.field8) {
        fields.value8.low = (params++)->ToInt();
        fields.value8.high = (params++)->ToInt();
        fields.value8.middle = (params++)->ToInt();
        fields.valueC.low = (params++)->ToInt();
        fields.valueC.middle = (params++)->ToInt();
        fields.value10.low = (params++)->ToInt();
        fields.valueC.high = (params++)->ToInt();
        fields.value10.middle = (params++)->ToInt();
        fields.value10.high = (params++)->ToInt();
        fields.value4High = (params++)->ToInt();
        CopyFiveFields020823e4(&((ResourceScriptEntry *) data_02108ee8.fieldC)->initial, &fields);
    }
    return 1;
}
