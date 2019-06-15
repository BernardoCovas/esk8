#include <e_ride_uart.h>

void e_ride_uart_msg_set_chk(
    e_ride_uart_msg_t* msg
)
{
    e_ride_uart_msg_chk_calc(*msg, msg->chk_sum);
}