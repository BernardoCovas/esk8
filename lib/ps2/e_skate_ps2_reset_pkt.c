#include "e_skate_ps2.h"

#include <driver/timer.h>


void e_skate_ps2_reset_pkt(

    e_skate_ps2_pkt_t* ps2Pkt

)
{
    ps2Pkt->newByte      = 0;
    ps2Pkt->frameIndex   = 0;
}