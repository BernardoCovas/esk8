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

e_skate_err_t e_skate_ps2_check_pkt(

    e_skate_ps2_pkt_t* ps2Pkt

)
{
    if (ps2Pkt->newStart != 1 ||
        ps2Pkt->newStop  != 0 )
        return E_SKATE_PS2_ERR_INVALID_STATE;

    return findParity(ps2Pkt->newByte)==ps2Pkt->newParity?
        E_SKATE_SUCCESS:
        E_SKATE_PS2_ERR_INVALID_STATE;
}
