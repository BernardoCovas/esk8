#include <e_skate_bms.h>

#include <stdio.h>


e_skate_bms_err_t e_skate_bms_msg_chk(
    e_skate_bms_msg_t msg
)
{
    uint8_t calcChkSum[2];
    e_skate_bms_msg_chk_calc(msg, calcChkSum);

    if (memcmp(calcChkSum, msg.msg_cheksum, sizeof(uint8_t) * 2) != 0)
        return E_SKATE_BMS_MSG_ERR_INVALID_CHKSUM;        

    return E_SKATE_BMS_MSG_SUCCESS;
}