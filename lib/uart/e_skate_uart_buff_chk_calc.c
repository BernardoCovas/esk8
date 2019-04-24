#include <e_skate_uart.h>

#include <stdio.h>


void e_skate_uart_buff_chk_calc(
    uint8_t* buffer,
    size_t   bufferLen,
    uint8_t  chkSumBuf[static 2]
)
{
    uint16_t chksum = 0;
    for (int i=0; i < bufferLen; i++)
        chksum += buffer[i];
    chksum ^= 0xFFFF;

    memcpy((void*) chkSumBuf, (void*) &chksum, 2);
}
