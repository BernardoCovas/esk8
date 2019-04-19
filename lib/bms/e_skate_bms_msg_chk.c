#include <e_skate_bms.h>

#include <stdio.h>


e_skate_bms_err_t e_skate_bms_msg_chk(
    e_skate_bms_msg_t msg
)
{
    size_t bufferLen = E_SKATE_MSG_HEADER_SIZE + msg.pld_length;

    uint8_t* buffer;
    buffer = (uint8_t*)malloc(bufferLen * sizeof(uint8_t));
    
    memcpy(
        buffer,
        (void*)&msg,
        E_SKATE_MSG_HEADER_SIZE * sizeof(uint8_t)
        );

    memcpy(
        buffer + E_SKATE_MSG_HEADER_SIZE,
        msg.payload,
        msg.pld_length * sizeof(uint8_t)
        );

    uint8_t calcChkSum[2];
    e_skate_bms_msg_chk_calc(buffer, bufferLen, calcChkSum);

    free(buffer);

    if (memcmp(calcChkSum, msg.chk_sum, sizeof(uint8_t) * 2) != 0)
        return E_SKATE_BMS_MSG_ERR_INVALID_CHKSUM;        

    return E_SKATE_BMS_MSG_SUCCESS;
}