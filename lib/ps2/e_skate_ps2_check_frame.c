#include <e_skate_err.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <stdbool.h>


bool findParity(uint8_t x)
{	
    x = (x & 0x0F)^(x >> 4);
    x = (x & 0x03)^(x >> 2);
    x = (x & 0x01)^(x >> 1);

    return ! (bool) (x & 1);
}

e_skate_err_t e_skate_ps2_check_frame(

    e_skate_ps2_handle_t* ps2Handle

)
{
    if (ps2Handle->newStart != 1 ||
        ps2Handle->newStop  != 0 )
        return E_SKATE_PS2_ERR_INVALID_STATE;

    return findParity(ps2Handle->newByte)==ps2Handle->newParity?
        E_SKATE_SUCCESS:
        E_SKATE_PS2_ERR_INVALID_STATE;
}
