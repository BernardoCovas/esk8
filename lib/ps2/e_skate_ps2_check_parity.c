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

e_skate_err_t e_skate_ps2_check_parity(

    e_skate_ps2_handle_t* ps2Handle,
    bool value

)
{
    return
    findParity(ps2Handle->newByte)==value?
        E_SKATE_SUCCESS
        :
        E_SKATE_PS2_ERR_INVALID_STATE;
}