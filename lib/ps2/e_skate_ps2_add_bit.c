#include "e_skate_ps2.h"
#include "e_skate_ps2_utils.h"

#include <stdbool.h>


e_skate_err_t e_skate_ps2_add_bit(

    e_skate_ps2_handle_t* ps2Handle,
    bool value

)
{
    int idx = ps2Handle->frameIndex;

    if (idx > 10)
        return E_SKATE_PS2_ERR_VALUE_READY;

    switch (idx)
    {
        case 0:
            ps2Handle->newStart     = value;
            break;
        case 9:
            ps2Handle->newParity    = value;
            break;
        case 10:
            ps2Handle->newStop      = value;
            ps2Handle->frameIndex++;
            return E_SKATE_PS2_ERR_VALUE_READY;
        default:
            goto ADD_BIT;
    }

    ps2Handle->frameIndex++;
    return E_SKATE_SUCCESS;

ADD_BIT:
    ps2Handle->newByte |= (value << (ps2Handle->frameIndex - 1));
    ps2Handle->frameIndex++;
    return E_SKATE_SUCCESS;
}
