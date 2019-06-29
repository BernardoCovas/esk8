#include <esk8_uart.h>

void esk8_uart_msg_serialize(
    esk8_uart_msg_t msg,
    uint8_t *buff
)
{
    const static uint8_t pktHeader[] = ESK8_MSG_PKT_HEADER;

    memcpy(buff, pktHeader, sizeof(pktHeader));

    buff += sizeof(pktHeader);
    memcpy(buff, (void*)&msg, ESK8_MSG_HEADER_SIZE);

    buff += ESK8_MSG_HEADER_SIZE;
    memcpy( buff, msg.payload, msg.pld_length);

    buff += msg.pld_length;
    memcpy( buff, msg.chk_sum, 2);
}