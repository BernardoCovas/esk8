
#include <e_skate_uart.h>

size_t e_skate_uart_msg_get_serialized_length(
    e_skate_uart_msg_t msg
)
{
    return msg.pld_length + E_SKATE_MSG_MIN_SIZE;
}
