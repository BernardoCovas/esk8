#include <e_skate_uart.h>


void e_skate_uart_msg_free(
    e_skate_uart_msg_t msg
)
{
    free(msg.payload);
}