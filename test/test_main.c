#include "test_e_skate.h"

#include <unity.h>

void app_main()
{
    UNITY_BEGIN();

    RUN_TEST(test_e_skate_bms_msg_find_header);
    RUN_TEST(test_e_skate_bms_buff_chk_calc);
    RUN_TEST(test_e_skate_bms_msg_chk_calc);
    RUN_TEST(test_e_skate_bms_msg_parse);
    RUN_TEST(test_e_skate_bms_msg_serialize);

    UNITY_END(); // stop unit testing
}