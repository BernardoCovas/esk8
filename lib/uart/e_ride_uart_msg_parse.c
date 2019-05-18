#include <e_ride_uart.h>
#include <e_ride_err.h>


e_ride_err_t
e_ride_uart_msg_parse(
    uint8_t* buffer,
    size_t buf_length,
    e_ride_uart_msg_t* msgOut
)
{
    int headerIndex = e_ride_uart_msg_find_header(
        buffer,
        buf_length);

    if (headerIndex < 0)
        return E_RIDE_UART_MSG_ERR_NO_HEADER;

    uint8_t* msgBuffer = buffer + headerIndex;
    uint8_t  remBufferLen = buf_length - headerIndex;

    if (remBufferLen < E_RIDE_MSG_MIN_SIZE)
        return E_RIDE_UART_MSG_ERR_INVALID_BUFFER;

    // Skip packet header
    msgBuffer += 2;
    remBufferLen -= 2;

    e_ride_uart_msg_t msgNew;
    memcpy((void*)&msgNew, msgBuffer, E_RIDE_MSG_HEADER_SIZE);

    msgBuffer += E_RIDE_MSG_HEADER_SIZE;
    remBufferLen -= E_RIDE_MSG_HEADER_SIZE;

    if(msgNew.pld_length + 2 > remBufferLen)
        return E_RIDE_UART_MSG_ERR_INVALID_PLDLEN;

    msgNew.payload = (uint8_t*)malloc(msgNew.pld_length);
    memcpy(msgNew.payload, msgBuffer, msgNew.pld_length);
    
    msgBuffer += msgNew.pld_length;
    remBufferLen -= msgNew.pld_length;
    memcpy(msgNew.chk_sum, msgBuffer, 2);

    e_ride_err_t errCode = e_ride_uart_msg_chk(msgNew);

    if (errCode != E_RIDE_SUCCESS)
    {
        free(msgNew.payload);
        return errCode;
    }

    (*msgOut) = msgNew;
    return E_RIDE_SUCCESS;
}
