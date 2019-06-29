#include <esk8_err.h>
#include <esk8_uart.h>

#include <stdio.h>


esk8_err_t esk8_uart_msg_chk(
    esk8_uart_msg_t msg
)
{
    uint8_t calcChkSum[2];
    esk8_uart_msg_chk_calc(msg, calcChkSum);

    if (memcmp(calcChkSum, msg.chk_sum, 2) != 0)
        return ESK8_UART_MSG_ERR_INVALID_CHKSUM;        

    return ESK8_SUCCESS;
}