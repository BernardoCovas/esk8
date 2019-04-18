#include "test_e_skate.h"

#include <e_skate_bms.h>

#include <unity.h>
#include <stdint.h>


void test_e_skate_bms_msg_new()
{
    uint8_t   valid_msg         [] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x25, 0xFF};
    uint8_t invalid_msg_pldlen  [] = {0x5A, 0xA5, 0x02, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF};
    uint8_t invalid_msg_cksum   [] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF};

    e_skate_bms_msg_t msg;
    e_skate_bms_err_t errCode;

    errCode = e_skate_bms_msg_new(valid_msg, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_BMS_MSG_SUCCESS, errCode);
    if(errCode == E_SKATE_BMS_MSG_SUCCESS)
        e_skate_bms_msg_free(msg);

    errCode = e_skate_bms_msg_new(invalid_msg_pldlen, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_BMS_MSG_ERR_INVALID_PLDLEN, errCode);
     if(errCode == E_SKATE_BMS_MSG_SUCCESS)
        e_skate_bms_msg_free(msg);

    errCode = e_skate_bms_msg_new(invalid_msg_cksum, 10, &msg);
    TEST_ASSERT_EQUAL(E_SKATE_BMS_MSG_ERR_INVALID_CHKSUM, errCode);
    if(errCode == E_SKATE_BMS_MSG_SUCCESS)
        e_skate_bms_msg_free(msg);
}
