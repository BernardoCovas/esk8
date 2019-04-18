#include "test_e_skate.h"

#include <unity.h>

void app_main()
{
    UNITY_BEGIN();

    RUN_TEST(test_e_skate_bms_msg_new);

    UNITY_END(); // stop unit testing
}