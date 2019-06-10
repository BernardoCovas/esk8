
#include <e_ride_uart.h>

size_t e_ride_uart_msg_get_serialized_length(
    e_ride_uart_msg_t msg
)
{
    return msg.pld_length + E_RIDE_MSG_MIN_SIZE;
}
