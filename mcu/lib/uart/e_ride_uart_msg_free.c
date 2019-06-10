#include <e_ride_uart.h>


void e_ride_uart_msg_free(
    e_ride_uart_msg_t msg
)
{
    free(msg.payload);
}