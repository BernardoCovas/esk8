#include <esk8_uart.h>
#include <esk8_err.h>


esk8_err_t
esk8_uart_regread_msg_new(
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

void
esk8_uart_msg_set_chk(
    esk8_uart_msg_t* msg
)
{
    esk8_uart_msg_chk_calc(*msg, msg->chk_sum);
}

void
esk8_uart_msg_serialize(
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

int
esk8_uart_msg_find_header(
    uint8_t* buffer,
    int buf_length
)
{
    const static uint8_t msg_header[] = ESK8_MSG_PKT_HEADER;

    if (buf_length < 2)
        return -1;

    for(int i=0; i<buf_length-1; i++)
    {
        int memcmp_res = memcmp(
            buffer+i,
            msg_header,
            2
        );

        if (memcmp_res == 0)
            return i;
    }

    return -1;
}

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
        return ESK8_ERR_UART_MSG_NO_HEADER;

    uint8_t* msgBuffer = buffer + headerIndex;
    uint8_t  remBufferLen = buf_length - headerIndex;

    if (remBufferLen < ESK8_MSG_MIN_SIZE)
        return ESK8_ERR_UART_MSG_INVALID_BUFFER;

    // Skip packet header
    msgBuffer += 2;
    remBufferLen -= 2;

    esk8_uart_msg_t msgNew;
    memcpy((void*)&msgNew, msgBuffer, ESK8_MSG_HEADER_SIZE);

    msgBuffer += ESK8_MSG_HEADER_SIZE;
    remBufferLen -= ESK8_MSG_HEADER_SIZE;

    if(msgNew.pld_length + 2 > remBufferLen)
        return ESK8_ERR_UART_MSG_INVALID_PLDLEN;

    msgNew.payload = (uint8_t*)malloc(msgNew.pld_length);
    memcpy(msgNew.payload, msgBuffer, msgNew.pld_length);

    msgBuffer += msgNew.pld_length;
    remBufferLen -= msgNew.pld_length;
    memcpy(msgNew.chk_sum, msgBuffer, 2);

    esk8_err_t errCode = esk8_uart_msg_chk(msgNew);

    if (errCode != ESK8_OK)
    {
        free(msgNew.payload);
        return errCode;
    }

    (*msgOut) = msgNew;
    return ESK8_OK;
}

size_t
esk8_uart_msg_get_serialized_length(
    esk8_uart_msg_t msg
)
{
    return msg.pld_length + ESK8_MSG_MIN_SIZE;
}

esk8_err_t
esk8_uart_msg_chk(
    esk8_uart_msg_t msg
)
{
    uint8_t calcChkSum[2];
    esk8_uart_msg_chk_calc(msg, calcChkSum);

    if (memcmp(calcChkSum, msg.chk_sum, 2) != 0)
        return ESK8_ERR_UART_MSG_INVALID_CHKSUM;

    return ESK8_OK;
}

void
esk8_uart_msg_chk_calc(
    esk8_uart_msg_t msg,
    uint8_t chkSum[static 2]
)
{
    size_t   buffSize = esk8_uart_msg_get_serialized_length(msg);
    uint8_t* buffer = (uint8_t*)malloc(buffSize);
    esk8_uart_msg_serialize(msg, buffer);
    esk8_uart_buff_chk_calc(buffer + 2, buffSize - 4, chkSum);
    free(buffer);
}


void
esk8_uart_buff_chk_calc(
    uint8_t* buffer,
    size_t   bufferLen,
    uint8_t  chkSumBuf[static 2]
)
{
    uint16_t chksum = 0;
    for (int i=0; i < bufferLen; i++)
        chksum += buffer[i];
    chksum ^= 0xFFFF;

    memcpy((void*) chkSumBuf, (void*) &chksum, 2);
}

void
esk8_uart_msg_free(
    esk8_uart_msg_t msg
)
{
    free(msg.payload);
}
