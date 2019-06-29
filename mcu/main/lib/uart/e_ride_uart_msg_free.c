#include <esk8_uart.h>


void esk8_uart_msg_free(
    esk8_uart_msg_t msg
)
{
    free(msg.payload);
}