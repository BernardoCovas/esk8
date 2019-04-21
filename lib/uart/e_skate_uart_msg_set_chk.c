#include <e_skate_uart.h>

void e_skate_uart_msg_set_chk(
    e_skate_uart_msg_t* msg
)
{
    e_skate_uart_msg_chk_calc(*msg, msg->chk_sum);
}