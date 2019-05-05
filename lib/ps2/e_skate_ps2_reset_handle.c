#include "e_skate_ps2.h"

#include <driver/timer.h>


void e_skate_ps2_reset_handle(

    e_skate_ps2_handle_t* ps2Handle,
    bool withTimer

)
{
    if (withTimer)
        timer_set_alarm_value(
            ps2Handle->ps2Config.timerConfig.timerGroup,
            ps2Handle->ps2Config.timerConfig.timerIdx,
            (uint64_t) 0);

    ps2Handle->newByte      = 0;
    ps2Handle->frameIndex   = 0;
}