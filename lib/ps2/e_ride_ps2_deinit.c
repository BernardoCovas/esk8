#include <e_ride_err.h>
#include <e_ride_ps2.h>
#include <e_ride_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>


e_ride_err_t e_ride_ps2_deinit(

    e_ride_ps2_handle_t* ps2Handle,
    bool                  withIsr

)
{
    if (ps2Handle->rxBitQueueHandle != NULL)
        vQueueDelete(ps2Handle->rxBitQueueHandle);

    if (ps2Handle->rxByteQueueHandle != NULL)
        vQueueDelete(ps2Handle->rxByteQueueHandle);
    
    if (ps2Handle->rxTaskHandle != NULL)
        vTaskDelete(ps2Handle->rxTaskHandle);

    timer_pause(
        ps2Handle->ps2Config.timerConfig.timerGroup,
        ps2Handle->ps2Config.timerConfig.timerIdx);

    ESP_ERROR_CHECK(gpio_isr_handler_remove(
        ps2Handle->ps2Config.gpioConfig.clockPin));

    if (withIsr)
        gpio_uninstall_isr_service();

    return E_RIDE_SUCCESS;
}