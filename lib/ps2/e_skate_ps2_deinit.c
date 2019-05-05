#include <e_skate_err.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>


e_skate_err_t e_skate_ps2_deinit(

    e_skate_ps2_handle_t* ps2Handle,
    bool                  withIsr

)
{
    vQueueDelete(ps2Handle->rxByteQueueHandle);

    ps2Handle->newByte = 0;
    ps2Handle->frameIndex = 0;

    timer_pause(
        ps2Handle->ps2Config.timerConfig.timerGroup,
        ps2Handle->ps2Config.timerConfig.timerIdx);

    if (withIsr)
        gpio_uninstall_isr_service();

    return E_SKATE_SUCCESS;
}