#include "test_e_skate.h"
#include <e_skate_uart.h>

#include <unity.h>


void test_e_skate_uart_buff_chk_calc()
{
    uint8_t   valid_msg[] = {0x01, 0x3E, 0x20, 0x02, 0x78, 0x01};
    uint8_t   expectedChkSum[] = {0x25, 0xFF};
    uint8_t   actualChkSum[2];

    e_skate_uart_buff_chk_calc(valid_msg, sizeof(valid_msg), actualChkSum);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedChkSum, actualChkSum, 2);
}