#include "test_e_skate.h"

#include <e_skate_bms.h>

#include <unity.h>
#include <stdint.h>


void test_e_skate_bms_msg_find_header()
{
    uint8_t some_header_at_0    [] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF};
    uint8_t some_header_at_4    [] = {0xFF, 0xA3, 0x23, 0x34, 0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x02, 0x78, 0x01, 0x24, 0xFF};
    uint8_t some_header_at_neg1 [] = {0xA5, 0x01, 0x3E, 0x20, 0x5A, 0x02, 0x78, 0x01, 0x24, 0xFF};

    int res1 = e_skate_bms_msg_find_header(some_header_at_0     , sizeof(some_header_at_0)    / sizeof(uint8_t));
    int res2 = e_skate_bms_msg_find_header(some_header_at_4     , sizeof(some_header_at_4)    / sizeof(uint8_t));
    int res3 = e_skate_bms_msg_find_header(some_header_at_neg1  , sizeof(some_header_at_neg1) / sizeof(uint8_t));

    TEST_ASSERT_EQUAL(res1,  0);
    TEST_ASSERT_EQUAL(res2,  4);
    TEST_ASSERT_EQUAL(res3, -1);
}
