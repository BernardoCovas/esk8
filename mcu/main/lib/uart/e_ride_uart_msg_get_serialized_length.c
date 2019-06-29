
#include <esk8_uart.h>

size_t esk8_uart_msg_get_serialized_length(
    esk8_uart_msg_t msg
)
{
    return msg.pld_length + ESK8_MSG_MIN_SIZE;
}
