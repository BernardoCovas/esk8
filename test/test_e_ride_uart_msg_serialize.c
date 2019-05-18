#include <e_ride_uart.h>

#include <unity.h>

#include "test_e_ride.h"

void test_e_ride_uart_msg_serialize()
{
    e_ride_uart_msg_t msg;
    e_ride_uart_regread_msg_new(
        E_RIDE_ADDR_BMS,
        E_RIDE_REG_BMS_CURRENT,
        0x02,
        &msg
    );

    int msgBufferLen = (int) e_ride_uart_msg_get_serialized_length(msg);

    uint8_t expectedBuffer[] = {0x5A, 0xA5, 0x01, 0x3e, 0x22, 0x01, E_RIDE_REG_BMS_CURRENT, 0x02, 0x68, 0xFF};
    uint8_t* actualBuffer = (uint8_t*)malloc(msgBufferLen);

    e_ride_uart_msg_serialize(msg, actualBuffer);

    TEST_ASSERT_EQUAL(sizeof(expectedBuffer), msgBufferLen);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, actualBuffer, msgBufferLen);

    e_ride_uart_msg_free(msg);
    free(actualBuffer);
}