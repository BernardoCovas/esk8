#include <e_skate_uart.h>

e_skate_uart_msg_err_t e_skate_uart_regread_msg_new(
    e_skate_uart_reg_t reg,
    uint8_t readLen,
    e_skate_uart_msg_t *outMsg
)
{
    e_skate_uart_msg_t newMsg = {
        .pld_length = 1,
        .src_address = 0x3e,
        .dst_address = 0x22,
        .cmd_command = 0x01,            // Read registers
        .cmd_argment = (uint8_t) reg,   // Reg to read
        .payload = (uint8_t*) malloc(1)
    };

    newMsg.payload[0] = readLen;
    e_skate_uart_msg_set_chk(&newMsg);

    (*outMsg) = newMsg;

    return E_SKATE_UART_MSG_SUCCESS;
}