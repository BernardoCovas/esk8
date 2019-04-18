#include <e_skate_bms.h>

#define min_msg_buffer_size 9


e_skate_bms_err_t
e_skate_bms_msg_new(
    uint8_t* buffer,
    size_t buf_length,
    e_skate_bms_msg_t* msgOut
)
{
    int headerIndex = e_skate_bms_msg_find_header(
        buffer,
        buf_length);

    if (headerIndex < 0)
        return E_SKATE_BMS_MSG_ERR_NO_HEADER;

    uint8_t* msgBuffer = buffer + headerIndex;
    uint8_t  msgBufferLen = buf_length - headerIndex;

    if (msgBufferLen < min_msg_buffer_size)
        return E_SKATE_BMS_MSG_ERR_INVALID_BUFFER;

    uint8_t advPayloadLen = msgBuffer[2];
    uint8_t remPayloadLen = msgBufferLen - min_msg_buffer_size;

    if(advPayloadLen > remPayloadLen)
        return E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN;

    msgBufferLen = min_msg_buffer_size + advPayloadLen;

    e_skate_bms_msg_t msgNew;

    size_t raw_buffer_size = sizeof(uint8_t) * msgBufferLen;
    msgNew.raw_buffer = (uint8_t*)malloc(raw_buffer_size);
    memcpy(msgNew.raw_buffer, msgBuffer, raw_buffer_size);

    msgNew.raw_buffer_len   = raw_buffer_size;
    msgNew.pld_length       = msgNew.raw_buffer  + 2;
    msgNew.src_address      = msgNew.raw_buffer  + 3;
    msgNew.dst_address      = msgNew.raw_buffer  + 4;
    msgNew.cmd_command      = msgNew.raw_buffer  + 5;
    msgNew.cmd_argment      = msgNew.raw_buffer  + 6;
    msgNew.msg_payload      = msgNew.raw_buffer  + 7;
    msgNew.msg_cheksum      = msgNew.msg_payload + advPayloadLen;

    e_skate_bms_err_t errCode = e_skate_bms_msg_chk(msgNew);

    if (errCode != E_SKATE_BMS_MSG_SUCCESS)
    {
        free(msgNew.raw_buffer);
        return errCode;
    }

    (*msgOut) = msgNew;
    return E_SKATE_BMS_MSG_SUCCESS;
}
