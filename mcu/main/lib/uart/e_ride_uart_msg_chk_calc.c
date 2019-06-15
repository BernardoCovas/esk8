#include <e_ride_uart.h>

void e_ride_uart_msg_chk_calc(
    e_ride_uart_msg_t msg,
    uint8_t chkSum[static 2]
)
{
    size_t   buffSize = e_ride_uart_msg_get_serialized_length(msg);
    uint8_t* buffer = (uint8_t*)malloc(buffSize);
    e_ride_uart_msg_serialize(msg, buffer);
    e_ride_uart_buff_chk_calc(buffer + 2, buffSize - 4, chkSum);
    free(buffer);
}