#include <esk8_err.h>
#include <esk8_ps2.h>
#include <esk8_ps2_utils.h>

#include <esp_intr_alloc.h>
#include <driver/gpio.h>
#include <driver/timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>


esk8_err_t esk8_ps2_deinit(

    esk8_ps2_handle_t* ps2Handle,
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

    return ESK8_SUCCESS;
}