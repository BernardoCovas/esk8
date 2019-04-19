#include <e_skate_bms.h>


void e_skate_bms_msg_free(
    e_skate_bms_msg_t msg
)
{
    free(msg.payload);
}