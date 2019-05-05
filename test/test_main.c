#include "test_e_skate.h"

#include <unity.h>

void app_main()
{
    UNITY_BEGIN();

    RUN_TEST(test_e_skate_uart_msg_find_header);
    RUN_TEST(test_e_skate_uart_buff_chk_calc);
    RUN_TEST(test_e_skate_uart_msg_chk_calc);
    RUN_TEST(test_e_skate_uart_msg_parse);
    RUN_TEST(test_e_skate_uart_msg_serialize);
    
    RUN_TEST(test_e_skate_ps2_add_bit);
    RUN_TEST(test_e_skate_ps2_check_frame);

    UNITY_END(); // stop unit testing
}