#include "test_e_skate.h"
#include <e_skate_uart.h>

#include <unity.h>


void test_e_skate_uart_msg_chk_calc()
{
    e_skate_uart_msg_t msgNew;
    e_skate_uart_regread_msg_new(
        E_SKATE_ADDR_BMS,
        E_SKATE_REG_BMS_CURRENT,
        0x02,
        &msgNew
    );

    uint8_t   expectedChkSum[] = {0x68, 0xFF};
    uint8_t   actualChkSum[2];

    e_skate_uart_msg_chk_calc(msgNew, actualChkSum);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedChkSum, actualChkSum, 2);

    e_skate_uart_msg_free(msgNew);
}