#include <e_skate_bms.h>


e_skate_bms_err_t
e_skate_bms_msg_new(
    uint8_t* buffer,
    size_t buf_length,
    e_skate_bms_msg_t* msgOut
)
{
    if (buf_length < 9)
        return E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN;

    e_skate_bms_msg_t msgNew;

    size_t raw_buffer_size = sizeof(uint8_t) * buf_length;
    msgNew.raw_buffer = (uint8_t*)malloc(raw_buffer_size);
    memcpy(msgNew.raw_buffer, buffer, raw_buffer_size);

    msgNew.raw_buffer_len   = buf_length;
    msgNew.pld_length       = msgNew.raw_buffer + 2;
    msgNew.src_address      = msgNew.raw_buffer + 3;
    msgNew.dst_address      = msgNew.raw_buffer + 4;
    msgNew.cmd_command      = msgNew.raw_buffer + 5;
    msgNew.cmd_argment      = msgNew.raw_buffer + 6;
    msgNew.msg_payload      = msgNew.raw_buffer + 7;
    msgNew.msg_cheksum      = msgNew.raw_buffer + buf_length - 2;

    e_skate_bms_err_t errCode = e_skate_bms_msg_chk(msgNew);

    if (errCode != E_SKATE_BMS_MSG_SUCCESS)
    {
        free(msgNew.raw_buffer);
        return errCode;
    }

    (*msgOut) = msgNew;
    return E_SKATE_BMS_MSG_SUCCESS;
}
