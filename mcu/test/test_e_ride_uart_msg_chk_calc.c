#include "test_e_ride.h"
#include <e_ride_uart.h>

#include <unity.h>


void test_e_ride_uart_msg_chk_calc()
{
    e_ride_uart_msg_t msgNew;
    e_ride_uart_regread_msg_new(
        E_RIDE_ADDR_BMS,
        E_RIDE_REG_BMS_CURRENT,
        0x02,
        &msgNew
    );

    uint8_t   expectedChkSum[] = {0x68, 0xFF};
    uint8_t   actualChkSum[2];

    e_ride_uart_msg_chk_calc(msgNew, actualChkSum);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedChkSum, actualChkSum, 2);

    e_ride_uart_msg_free(msgNew);
}