#include <e_ride_err.h>
#include <e_ride_uart.h>

#include <stdio.h>


e_ride_err_t e_ride_uart_msg_chk(
    e_ride_uart_msg_t msg
)
{
    uint8_t calcChkSum[2];
    e_ride_uart_msg_chk_calc(msg, calcChkSum);

    if (memcmp(calcChkSum, msg.chk_sum, 2) != 0)
        return E_RIDE_UART_MSG_ERR_INVALID_CHKSUM;        

    return E_RIDE_SUCCESS;
}