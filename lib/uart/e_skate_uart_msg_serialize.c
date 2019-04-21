#include <e_skate_uart.h>

void e_skate_uart_msg_serialize(
    e_skate_uart_msg_t msg,
    uint8_t *buff
)
{
    const static uint8_t pktHeader[] = E_SKATE_MSG_PKT_HEADER;

    memcpy(
        buff,
        pktHeader,
        sizeof(pktHeader)
        );

    buff += sizeof(pktHeader);
    
    memcpy(
        buff,
        (void*)&msg,
        E_SKATE_MSG_HEADER_SIZE
        );

    buff += E_SKATE_MSG_HEADER_SIZE;

    memcpy(
        buff,
        msg.payload,
        msg.pld_length
    );

    buff += msg.pld_length;

    memcpy(
        buff,
        msg.chk_sum,
        2        
    );

}