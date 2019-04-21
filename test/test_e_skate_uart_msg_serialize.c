#include <e_skate_uart.h>

#include <unity.h>

#include "test_e_skate.h"

void test_e_skate_uart_msg_serialize()
{
    e_skate_uart_msg_t msg;
    e_skate_uart_msg_new(
        E_SKATE_BMS_REG_CURRENT,
        &msg
    );

    int msgBufferLen = (int) e_skate_uart_msg_get_serialized_length(msg);

    uint8_t expectedBuffer[] = {0x5A, 0xA5, 0x01, 0x20, 0x22, 0x01, E_SKATE_BMS_REG_CURRENT, 0x02, 0x86, 0xFF};
    uint8_t* actualBuffer = (uint8_t*)malloc(msgBufferLen);

    e_skate_uart_msg_serialize(msg, actualBuffer);

    TEST_ASSERT_EQUAL(sizeof(expectedBuffer), msgBufferLen);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, actualBuffer, msgBufferLen);

    e_skate_uart_msg_free(msg);
    free(actualBuffer);
}