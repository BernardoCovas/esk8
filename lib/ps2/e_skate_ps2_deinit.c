#include <e_skate_err.h>
#include <e_skate_ps2.h>
#include <e_skate_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>


e_skate_err_t e_skate_ps2_deinit(

    e_skate_ps2_handle_t* ps2Handle,
    bool                  withIsr

)
{

    if (ps2Handle->rxBitQueueHandle != NULL)
        vQueueDelete(ps2Handle->rxBitQueueHandle);

    if (ps2Handle->rxByteQueueHandle != NULL)
        vQueueDelete(ps2Handle->rxByteQueueHandle);
    
    if (ps2Handle->rxTaskHandle != NULL)
        vTaskDelete(ps2Handle->rxTaskHandle);

    // TODO Tx task

    timer_pause(
        ps2Handle->ps2Config.timerConfig.timerGroup,
        ps2Handle->ps2Config.timerConfig.timerIdx);

    if (withIsr)
        gpio_uninstall_isr_service();

    return E_SKATE_SUCCESS;
}