#include <e_skate_bms.h>

void e_skate_bms_msg_set_chk(
    e_skate_bms_msg_t* msg
)
{
    e_skate_bms_msg_chk_calc(*msg, msg->chk_sum);
}