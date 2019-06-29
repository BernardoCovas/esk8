#include <esk8_uart.h>

void esk8_uart_msg_set_chk(
    esk8_uart_msg_t* msg
)
{
    esk8_uart_msg_chk_calc(*msg, msg->chk_sum);
}