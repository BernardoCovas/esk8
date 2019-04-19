#include <e_skate_bms.h>

#include <stdio.h>


void e_skate_bms_buff_chk_calc(
    uint8_t* buffer,
    size_t   bufferLen,
    uint8_t  chkSumBuf[static 2]
)
{
    uint16_t chksum = 0;
    for (int i=0; i < bufferLen; i++)
        chksum += buffer[i];
    chksum ^= 0xFFFF;

    uint8_t *chksum_l = (uint8_t*) (void*) &chksum;

    chkSumBuf[0] = chksum_l[0];
    chkSumBuf[1] = chksum_l[1];
}
