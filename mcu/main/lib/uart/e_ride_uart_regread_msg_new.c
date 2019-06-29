#include <esk8_uart.h>
#include <esk8_err.h>


esk8_err_t esk8_uart_regread_msg_new(

    esk8_uart_addr_t dstAddr,
    esk8_uart_reg_t reg,
    uint8_t readLen,
    esk8_uart_msg_t *outMsg

)
{
    esk8_uart_msg_t newMsg = {
        .pld_length = 1,
        .src_address = 0x3e,
        .dst_address = dstAddr,
        .cmd_command = 0x01,            // Read registers
        .cmd_argment = (uint8_t) reg,   // Reg to read
        .payload = (uint8_t*) malloc(1)
    };

    newMsg.payload[0] = readLen;
    esk8_uart_msg_set_chk(&newMsg);

    (*outMsg) = newMsg;

    return EXIT_SUCCESS;
}