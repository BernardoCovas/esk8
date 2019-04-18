#include <e_skate_bms.h>

#include <stdio.h>


void e_skate_bms_msg_chk_calc(
    e_skate_bms_msg_t msg,
    uint8_t chkSumBuf[static 2]
)
{
    uint16_t chksum = 0;
    for (uint8_t* i=msg.pld_length; i<msg.msg_cheksum; i++)
        chksum += *i;
    chksum ^= 0xFFFF;

    uint8_t *chksum_l = (uint8_t*) (void*) &chksum;

    chkSumBuf[0] = chksum_l[0];
    chkSumBuf[1] = chksum_l[1];
}
