#include "e_skate_ps2.h"
#include "e_skate_ps2_utils.h"

#include <stdbool.h>
#include <stdint.h>


e_skate_err_t e_skate_ps2_add_bit(

    e_skate_ps2_pkt_t* ps2Pkt,
    bool value

)
{
    int idx = ps2Pkt->frameIndex;
    e_skate_err_t errCode = E_SKATE_SUCCESS;

    if (idx > 10)
        return E_SKATE_PS2_ERR_VALUE_READY;

    switch (idx)
    {
        case 0:
            ps2Pkt->newStart     = value;
            break;
        case 9:
            ps2Pkt->newParity    = value;
            break;
        case 10:
            ps2Pkt->newStop      = value;
            errCode  = E_SKATE_PS2_ERR_VALUE_READY;
            break;
        default:
            goto ADD_BIT;
    }

    ++(ps2Pkt->frameIndex);
    return errCode;

ADD_BIT:
    ps2Pkt->newByte |= (value << (idx - 1));
    ++(ps2Pkt->frameIndex);
    return E_SKATE_SUCCESS;
}


e_skate_err_t e_skate_ps2_take_bit(

    e_skate_ps2_pkt_t*  ps2Pkt,
    bool*               outValue

)
{
    int idx = ps2Pkt->frameIndex;
    e_skate_err_t errCode = E_SKATE_SUCCESS;

    if (idx > 10)
        return E_SKATE_PS2_ERR_VALUE_READY;

    switch (idx)
    {
        case 0:
            (*outValue) = ps2Pkt->newStart;
            break;
        case 9:
            (*outValue) = ps2Pkt->newParity;
            break;
        case 10:
            (*outValue) = ps2Pkt->newStop;
            errCode = E_SKATE_PS2_ERR_VALUE_READY;
            break;
        default:
            goto GET_BIT;
    }

    ++(ps2Pkt->frameIndex);
    return errCode;

GET_BIT:
    (*outValue) = (bool) (ps2Pkt->newByte & (1 << (idx - 1) ));
    ++(ps2Pkt->frameIndex);
    return E_SKATE_SUCCESS;
}


void e_skate_ps2_reset_pkt(

    e_skate_ps2_pkt_t* ps2Pkt

)
{
    ps2Pkt->newByte      = 0;
    ps2Pkt->frameIndex   = 0;
}


bool e_skate_ps2_get_parity(uint8_t x)
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
    if (ps2Pkt->newStart != 0 ||
        ps2Pkt->newStop  != 1 )
        return E_SKATE_PS2_ERR_INVALID_STATE;

    return e_skate_ps2_get_parity(ps2Pkt->newByte)==ps2Pkt->newParity?
        E_SKATE_SUCCESS:
        E_SKATE_PS2_ERR_INVALID_STATE;
}
