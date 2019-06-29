#include <esk8_uart.h>
#include <esk8_err.h>


esk8_err_t
esk8_uart_msg_parse(
    uint8_t* buffer,
    size_t buf_length,
    esk8_uart_msg_t* msgOut
)
{
    int headerIndex = esk8_uart_msg_find_header(
        buffer,
        buf_length);

    if (headerIndex < 0)
        return ESK8_UART_MSG_ERR_NO_HEADER;

    uint8_t* msgBuffer = buffer + headerIndex;
    uint8_t  remBufferLen = buf_length - headerIndex;

    if (remBufferLen < ESK8_MSG_MIN_SIZE)
        return ESK8_UART_MSG_ERR_INVALID_BUFFER;

    // Skip packet header
    msgBuffer += 2;
    remBufferLen -= 2;

    esk8_uart_msg_t msgNew;
    memcpy((void*)&msgNew, msgBuffer, ESK8_MSG_HEADER_SIZE);

    msgBuffer += ESK8_MSG_HEADER_SIZE;
    remBufferLen -= ESK8_MSG_HEADER_SIZE;

    if(msgNew.pld_length + 2 > remBufferLen)
        return ESK8_UART_MSG_ERR_INVALID_PLDLEN;

    msgNew.payload = (uint8_t*)malloc(msgNew.pld_length);
    memcpy(msgNew.payload, msgBuffer, msgNew.pld_length);
    
    msgBuffer += msgNew.pld_length;
    remBufferLen -= msgNew.pld_length;
    memcpy(msgNew.chk_sum, msgBuffer, 2);

    esk8_err_t errCode = esk8_uart_msg_chk(msgNew);

    if (errCode != ESK8_SUCCESS)
    {
        free(msgNew.payload);
        return errCode;
    }

    (*msgOut) = msgNew;
    return ESK8_SUCCESS;
}
