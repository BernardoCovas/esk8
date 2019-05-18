#include "test_e_ride.h"

#include <unity.h>

void app_main()
{
    UNITY_BEGIN();

    RUN_TEST(test_e_ride_uart_msg_find_header);
    RUN_TEST(test_e_ride_uart_buff_chk_calc);
    RUN_TEST(test_e_ride_uart_msg_chk_calc);
    RUN_TEST(test_e_ride_uart_msg_parse);
    RUN_TEST(test_e_ride_uart_msg_serialize);
    
    RUN_TEST(test_e_ride_ps2_add_bit);
    RUN_TEST(test_e_ride_ps2_take_bit);
    RUN_TEST(test_e_ride_ps2_check_frame);

    UNITY_END(); // stop unit testing
}