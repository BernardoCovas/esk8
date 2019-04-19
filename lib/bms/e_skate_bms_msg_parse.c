#include <e_skate_bms.h>


e_skate_bms_err_t
e_skate_bms_msg_parse(
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
    uint8_t  remBufferLen = buf_length - headerIndex;

    if (remBufferLen < E_SKATE_MSG_MIN_SIZE)
        return E_SKATE_BMS_MSG_ERR_INVALID_BUFFER;

    // Skip packet header
    msgBuffer += 2;
    remBufferLen -= 2;

    e_skate_bms_msg_t msgNew;
    memcpy((void*)&msgNew, msgBuffer, sizeof(uint8_t)*E_SKATE_MSG_HEADER_SIZE);

    msgBuffer += E_SKATE_MSG_HEADER_SIZE;
    remBufferLen -= E_SKATE_MSG_HEADER_SIZE;

    if(msgNew.pld_length + 2 > remBufferLen)
        return E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN;

    msgNew.payload = (uint8_t*)malloc(sizeof(uint8_t) * msgNew.pld_length);
    memcpy(msgNew.payload, msgBuffer, sizeof(uint8_t) * msgNew.pld_length);
    
    msgBuffer += msgNew.pld_length;
    remBufferLen -= msgNew.pld_length;
    memcpy(msgNew.chk_sum, msgBuffer, sizeof(uint8_t) * 2);

    e_skate_bms_err_t errCode = e_skate_bms_msg_chk(msgNew);

    if (errCode != E_SKATE_BMS_MSG_SUCCESS)
    {
        free(msgNew.payload);
        return errCode;
    }

    (*msgOut) = msgNew;
    return E_SKATE_BMS_MSG_SUCCESS;
}
