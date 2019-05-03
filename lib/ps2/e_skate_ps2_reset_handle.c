#include "e_skate_ps2_utils.h"

void e_skate_ps2_reset_handle(

    e_skate_ps2_handle_t* ps2Handle

)
{
    ps2Handle->newByte = 0;
    ps2Handle->newByteIndex = 0;
}