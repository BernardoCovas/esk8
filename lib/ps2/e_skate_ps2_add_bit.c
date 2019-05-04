#include "e_skate_ps2.h"
#include "e_skate_ps2_utils.h"

#include <stdbool.h>

/* TODO (bcovas): Test */
e_skate_err_t e_skate_ps2_add_bit(

    e_skate_ps2_handle_t* ps2Handle,
    bool value

)
{
    if (ps2Handle->newByteIndex > 7)
        return E_SKATE_PS2_ERR_VALUE_READY;

    ps2Handle->newByte |= (value << ps2Handle->newByteIndex);
    ps2Handle->newByteIndex++;
    
    return ps2Handle->newByteIndex>7?E_SKATE_PS2_ERR_VALUE_READY:E_SKATE_SUCCESS;
}
