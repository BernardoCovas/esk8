#include <e_ride_uart.h>
#include <e_ride_err.h>


e_ride_err_t e_ride_uart_regread_msg_new(

    e_ride_uart_addr_t dstAddr,
    e_ride_uart_reg_t reg,
    uint8_t readLen,
    e_ride_uart_msg_t *outMsg

)
{
    e_ride_uart_msg_t newMsg = {
        .pld_length = 1,
        .src_address = 0x3e,
        .dst_address = dstAddr,
        .cmd_command = 0x01,            // Read registers
        .cmd_argment = (uint8_t) reg,   // Reg to read
        .payload = (uint8_t*) malloc(1)
    };

    newMsg.payload[0] = readLen;
    e_ride_uart_msg_set_chk(&newMsg);

    (*outMsg) = newMsg;

    return EXIT_SUCCESS;
}