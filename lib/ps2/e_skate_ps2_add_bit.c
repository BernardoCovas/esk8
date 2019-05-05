#include "e_skate_ps2.h"
#include "e_skate_ps2_utils.h"

#include <stdbool.h>


e_skate_err_t e_skate_ps2_add_bit(

    e_skate_ps2_pkt_t* ps2Pkt,
    bool value

)
{
    int idx = ps2Pkt->frameIndex;

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
            ps2Pkt->frameIndex++;
            return E_SKATE_PS2_ERR_VALUE_READY;
        default:
            goto ADD_BIT;
    }

    ps2Pkt->frameIndex++;
    return E_SKATE_SUCCESS;

ADD_BIT:
    ps2Pkt->newByte |= (value << (ps2Pkt->frameIndex - 1));
    ps2Pkt->frameIndex++;
    return E_SKATE_SUCCESS;
}
