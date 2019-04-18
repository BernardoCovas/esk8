#include <e_skate_bms.h>

#include <stdio.h>


e_skate_bms_err_t e_skate_bms_msg_chk(
    e_skate_bms_msg_t msg
)
{
    const static uint8_t msg_header[] = E_SKATE_MSG_HEADER;

    if (msg.raw_buffer_len < 9)
        return E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN;

    if (memcmp(msg.raw_buffer, msg_header, sizeof(uint8_t) * 2) != 0)
        return E_SKATE_BMS_MSG_ERR_NO_HEADER;

    uint8_t calcChkSum[2];
    e_skate_bms_msg_chk_calc(msg, calcChkSum);

    if (memcmp(calcChkSum, msg.msg_cheksum, sizeof(uint8_t) * 2) != 0)
        return E_SKATE_BMS_MSG_ERR_INVALID_CHKSUM;        

    return E_SKATE_BMS_MSG_SUCCESS;
}